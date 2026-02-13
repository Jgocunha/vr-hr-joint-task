#!/usr/bin/env Rscript

suppressPackageStartupMessages({
  library(data.table)
  library(stringr)
  library(lubridate)
})

# -----------------------------
# Config
# -----------------------------
base_dir <- "."
hits_dir <- file.path(base_dir, "data", "controller_hits")
inf_dir  <- file.path(base_dir, "data", "human_action_inferred")

# Max allowed time difference (seconds) between a hit and its matched inference
max_window_sec <- 1.0

# If filename-based pairing fails, allow pairing by closest first-timestamp within this many seconds
max_pairing_gap_sec <- 120

# -----------------------------
# Helpers
# -----------------------------

# Parse a "session start time" from filename (supports patterns like:
#   controller_hits_2026-02-13_11-33-24.txt
#   human_action_inferred26-02-13_11h33m23s.txt
parse_start_time_from_filename <- function(path) {
  fn <- basename(path)
  
  # Pattern A: YYYY-MM-DD_HH-MM-SS
  m1 <- str_match(fn, "(\\d{4}-\\d{2}-\\d{2})_(\\d{2})-(\\d{2})-(\\d{2})")
  if (!all(is.na(m1))) {
    dt <- paste0(m1[2], " ", m1[3], ":", m1[4], ":", m1[5])
    return(ymd_hms(dt, tz = "UTC", quiet = TRUE))
  }
  
  # Pattern B: YY-MM-DD_HHhMMmSSs  (note: year is 2-digit here)
  m2 <- str_match(fn, "(\\d{2})-(\\d{2})-(\\d{2})_(\\d{2})h(\\d{2})m(\\d{2})s")
  if (!all(is.na(m2))) {
    yy <- as.integer(m2[2])
    yyyy <- 2000 + yy
    dt <- sprintf("%04d-%02d-%02d %02d:%02d:%02d",
                  yyyy, as.integer(m2[3]), as.integer(m2[4]),
                  as.integer(m2[5]), as.integer(m2[6]), as.integer(m2[7]))
    return(ymd_hms(dt, tz = "UTC", quiet = TRUE))
  }
  
  return(as.POSIXct(NA))
}

read_lines_safely <- function(path) {
  # Avoid warnings if file ends weirdly
  tryCatch(readLines(path, warn = FALSE), error = function(e) character())
}

# Parse controller hits file into data.table:
# columns: file_hit, hit_time, simTime, hit_object
parse_controller_hits <- function(path) {
  lines <- read_lines_safely(path)
  if (length(lines) == 0) return(data.table())
  
  # Example:
  # [2026-02-13 11:33:34.801000] simTime=41.750  hit=object_1
  rx <- "^\\[(\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d+)\\]\\s+simTime=([0-9.]+)\\s+hit=(object_\\d+)"
  
  m <- str_match(lines, rx)
  m <- m[!is.na(m[,1]), , drop = FALSE]
  if (nrow(m) == 0) return(data.table())
  
  dt <- data.table(
    file_hit   = path,
    hit_time   = ymd_hms(m[,2], tz = "UTC", quiet = TRUE),
    simTime    = as.numeric(m[,3]),
    hit_object = m[,4]
  )
  dt <- dt[!is.na(hit_time)]
  dt[]
}

# Parse inferred actions file into data.table:
# columns: file_inf, inf_time, inferred_action
parse_inferred <- function(path) {
  lines <- read_lines_safely(path)
  if (length(lines) == 0) return(data.table())
  
  # Example (note there may be no space between action id and timestamp):
  # Human action inferred: 12026-02-13 11:32:13.240087
  rx <- "^Human action inferred:\\s*(\\d)\\s*(\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}\\.\\d+)$"
  
  m <- str_match(lines, rx)
  m <- m[!is.na(m[,1]), , drop = FALSE]
  if (nrow(m) == 0) return(data.table())
  
  dt <- data.table(
    file_inf        = path,
    inferred_action = as.integer(m[,2]),
    inf_time        = ymd_hms(m[,3], tz = "UTC", quiet = TRUE)
  )
  dt <- dt[!is.na(inf_time)]
  dt[]
}

first_timestamp_in_dt <- function(dt, time_col) {
  if (nrow(dt) == 0) return(as.POSIXct(NA))
  min(dt[[time_col]], na.rm = TRUE)
}

match_hits_to_inferences <- function(hits_dt, inf_dt, max_window_sec = 1.0, one_to_one = TRUE) {
  if (nrow(hits_dt) == 0) return(data.table())
  
  # Ensure hit_action exists (1/2/3 from object_1/object_2/object_3)
  if (!"hit_action" %in% names(hits_dt)) {
    hits_dt <- copy(hits_dt)
    hits_dt[, hit_action := as.integer(stringr::str_extract(hit_object, "\\d+"))]
  }
  
  # If no inference rows, return hits with NA inference columns
  if (nrow(inf_dt) == 0) {
    out <- copy(hits_dt)
    out[, `:=`(
      file_inf = NA_character_,
      inferred_action = NA_integer_,
      inf_time = as.POSIXct(NA),
      dt_seconds = NA_real_,
      matched = FALSE
    )]
    return(out[])
  }
  
  hits_dt <- copy(hits_dt)
  inf_dt  <- copy(inf_dt)
  
  setorder(hits_dt, hit_time)
  setorder(inf_dt, inf_time)
  
  if (one_to_one) inf_dt[, used := FALSE]
  
  out_list <- vector("list", nrow(hits_dt))
  
  for (i in seq_len(nrow(hits_dt))) {
    ht <- hits_dt$hit_time[i]
    ha <- hits_dt$hit_action[i]
    
    # Candidates:
    # - same object/action
    # - inference time strictly before hit time
    # - within max_window_sec
    # - not used (if one_to_one)
    cand <- inf_dt[
      inferred_action == ha &
        inf_time < ht &
        as.numeric(difftime(ht, inf_time, units = "secs")) <= max_window_sec
    ]
    
    if (one_to_one) cand <- cand[used == FALSE]
    
    if (nrow(cand) == 0) {
      out_list[[i]] <- cbind(
        hits_dt[i],
        data.table(
          file_inf = NA_character_,
          inferred_action = NA_integer_,
          inf_time = as.POSIXct(NA),
          dt_seconds = NA_real_,
          matched = FALSE
        )
      )
      next
    }
    
    # Choose the closest inference BEFORE hit => maximize inf_time (equivalently minimize ht - inf_time)
    cand[, dt_seconds := as.numeric(difftime(ht, inf_time, units = "secs"))]
    setorder(cand, dt_seconds, -inf_time)  # smallest lag first
    
    pick <- cand[1]
    
    if (one_to_one) {
      inf_dt[file_inf == pick$file_inf & inf_time == pick$inf_time & inferred_action == pick$inferred_action, used := TRUE]
    }
    
    out_list[[i]] <- cbind(
      hits_dt[i],
      data.table(
        file_inf = pick$file_inf,
        inferred_action = pick$inferred_action,
        inf_time = pick$inf_time,
        dt_seconds = pick$dt_seconds,  # this is hit_time - inf_time (positive)
        matched = TRUE
      )
    )
  }
  
  out <- rbindlist(out_list, use.names = TRUE, fill = TRUE)
  if (one_to_one) inf_dt[, used := NULL]
  out[]
}


# -----------------------------
# Load files
# -----------------------------
hit_files <- list.files(hits_dir, pattern = "\\.txt$", full.names = TRUE)
inf_files <- list.files(inf_dir,  pattern = "\\.txt$", full.names = TRUE)

if (length(hit_files) == 0) stop(sprintf("No hit files found in: %s", hits_dir))
if (length(inf_files) == 0) stop(sprintf("No inference files found in: %s", inf_dir))

hits_all <- rbindlist(lapply(hit_files, parse_controller_hits), fill = TRUE)
inf_all  <- rbindlist(lapply(inf_files,  parse_inferred), fill = TRUE)

if (nrow(hits_all) == 0) stop("Parsed 0 hit rows (check controller log format).")
if (nrow(inf_all) == 0)  stop("Parsed 0 inference rows (check inference log format).")

# -----------------------------
# Pair hit-files to inference-files by session start times
# -----------------------------
hit_meta <- data.table(file_hit = hit_files)
hit_meta[, start_fn := parse_start_time_from_filename(file_hit)]
hit_meta[, hits_first_ts := as.POSIXct(NA)]
for (i in seq_len(nrow(hit_meta))) {
  dt_i <- hits_all[file_hit == hit_meta$file_hit[i]]
  hit_meta$hits_first_ts[i] <- first_timestamp_in_dt(dt_i, "hit_time")
}
hit_meta[, start_time := fifelse(!is.na(start_fn), start_fn, hits_first_ts)]

inf_meta <- data.table(file_inf = inf_files)
inf_meta[, start_fn := parse_start_time_from_filename(file_inf)]
inf_meta[, inf_first_ts := as.POSIXct(NA)]
for (i in seq_len(nrow(inf_meta))) {
  dt_i <- inf_all[file_inf == inf_meta$file_inf[i]]
  inf_meta$inf_first_ts[i] <- first_timestamp_in_dt(dt_i, "inf_time")
}
inf_meta[, start_time := fifelse(!is.na(start_fn), start_fn, inf_first_ts)]

# All pairwise diffs, then choose closest for each hit file
pair_grid <- CJ(file_hit = hit_meta$file_hit, file_inf = inf_meta$file_inf)
pair_grid <- merge(pair_grid, hit_meta[, .(file_hit, hit_start = start_time)], by = "file_hit", all.x = TRUE)
pair_grid <- merge(pair_grid, inf_meta[, .(file_inf, inf_start = start_time)], by = "file_inf", all.x = TRUE)
pair_grid[, start_gap_sec := abs(as.numeric(difftime(inf_start, hit_start, units = "secs")))]

setorder(pair_grid, file_hit, start_gap_sec)
best_pairs <- pair_grid[, .SD[1], by = file_hit]
best_pairs[, paired_ok := !is.na(start_gap_sec) & start_gap_sec <= max_pairing_gap_sec]

# -----------------------------
# Match within each paired session
# -----------------------------
matched_sessions <- vector("list", nrow(best_pairs))
for (i in seq_len(nrow(best_pairs))) {
  hf <- best_pairs$file_hit[i]
  inf_f <- best_pairs$file_inf[i]
  ok <- best_pairs$paired_ok[i]
  
  hits_dt <- hits_all[file_hit == hf]
  inf_dt  <- if (ok) inf_all[file_inf == inf_f] else data.table()
  
  m <- match_hits_to_inferences(hits_dt, inf_dt, max_window_sec = max_window_sec)
  m[, `:=`(
    session_hit_file = basename(hf),
    session_inf_file = if (ok) basename(inf_f) else NA_character_,
    pairing_gap_sec  = best_pairs$start_gap_sec[i],
    pairing_ok       = ok
  )]
  matched_sessions[[i]] <- m
}

matched <- rbindlist(matched_sessions, fill = TRUE)

# Optional: add a normalized "hit_action" (1/2/3) to compare directly to inferred_action
matched[, hit_action := as.integer(str_extract(hit_object, "\\d+"))]
matched[, action_agrees := !is.na(inferred_action) & (hit_action == inferred_action)]

# -----------------------------
# Output
# -----------------------------
setcolorder(matched, c(
  "session_hit_file","session_inf_file","pairing_ok","pairing_gap_sec",
  "hit_time","simTime","hit_object","hit_action",
  "inf_time","inferred_action","dt_seconds","matched","action_agrees",
  "file_hit","file_inf"
))

out_csv <- file.path(base_dir, "matched_hits_inference.csv")
fwrite(matched, out_csv)

cat(sprintf("Wrote %d rows to %s\n", nrow(matched), out_csv))
cat(sprintf("Matched (within %.3fs): %d / %d\n",
            max_window_sec, sum(matched$matched, na.rm = TRUE), nrow(matched)))

# -----------------------------
# Export simplified CSV
# -----------------------------

simple_out <- matched[matched == TRUE, .(
  hit_object,
  hit_action,
  hit_time,
  inf_time,
  dt_second = dt_seconds   # rename column here
)]

out_csv_simple <- file.path(base_dir, "matched_hits_inference_simple.csv")
fwrite(simple_out, out_csv_simple)

cat(sprintf("Wrote simplified table to %s\n", out_csv_simple))

# -----------------------------
# Timing statistics
# -----------------------------

# Use only successful matches
timing <- matched[matched == TRUE & !is.na(dt_seconds)]

if (nrow(timing) == 0) {
  cat("No valid matches found for statistics.\n")
} else {
  
  stats_overall <- timing[, .(
    n_matches   = .N,
    median_sec  = median(dt_seconds),
    mean_sec    = mean(dt_seconds),
    sd_sec      = sd(dt_seconds),
    min_sec     = min(dt_seconds),
    max_sec     = max(dt_seconds),
    p25_sec     = quantile(dt_seconds, 0.25),
    p75_sec     = quantile(dt_seconds, 0.75),
    mad_sec     = mad(dt_seconds)   # robust variability metric
  )]
  
  print(stats_overall)
  
  fwrite(stats_overall, file.path(base_dir, "timing_stats_overall.csv"))
}

# -----------------------------
# Latency distribution diagnostics
# -----------------------------

if (nrow(timing) > 0) {
  
  # We use e1071 for skewness/kurtosis
  if (!requireNamespace("e1071", quietly = TRUE)) {
    install.packages("e1071")
  }
  library(e1071)
  
  diag_stats <- timing[, .(
    n_matches = .N,
    mean_sec  = mean(dt_seconds),
    median_sec= median(dt_seconds),
    sd_sec    = sd(dt_seconds),
    skewness  = e1071::skewness(dt_seconds, type = 2),
    kurtosis  = e1071::kurtosis(dt_seconds, type = 2)
  )]
  
  print(diag_stats)
  fwrite(diag_stats, file.path(base_dir, "timing_distribution_diagnostics.csv"))
}

# -----------------------------
# Outlier detection
# -----------------------------

if (nrow(timing) > 0) {
  
  # --- Method 1: IQR rule ---
  q1 <- quantile(timing$dt_seconds, 0.25)
  q3 <- quantile(timing$dt_seconds, 0.75)
  iqr <- q3 - q1
  
  lower_iqr <- q1 - 1.5 * iqr
  upper_iqr <- q3 + 1.5 * iqr
  
  timing[, outlier_iqr := dt_seconds < lower_iqr | dt_seconds > upper_iqr]
  
  # --- Method 2: Robust MAD z-score ---
  med <- median(timing$dt_seconds)
  mad_val <- mad(timing$dt_seconds)
  
  timing[, robust_z := (dt_seconds - med) / mad_val]
  timing[, outlier_mad := abs(robust_z) > 3]   # threshold ~3 is common
  
  # Combine flags
  timing[, outlier_any := outlier_iqr | outlier_mad]
  
  # Save flagged rows
  fwrite(
    timing[outlier_any == TRUE, .(
      hit_object,
      hit_action,
      hit_time,
      inf_time,
      dt_seconds,
      robust_z,
      outlier_iqr,
      outlier_mad
    )],
    file.path(base_dir, "timing_outliers.csv")
  )
  
  cat(sprintf("Outliers detected: %d / %d\n",
              sum(timing$outlier_any), nrow(timing)))
}

library(ggplot2)


plots_dir <- file.path(base_dir, "plots")
dir.create(plots_dir, showWarnings = FALSE, recursive = TRUE)

# Latency Distribution

p_hist <- ggplot(timing, aes(x = dt_seconds)) +
  geom_histogram(binwidth = 0.05) +
  geom_vline(aes(xintercept = median(dt_seconds)), linetype = "dashed") +
  labs(
    title = "Distribution of Anticipation Latency",
    x = "Seconds inference occurs before hit",
    y = "Count"
  ) +
  theme_minimal()

ggsave(
  filename = file.path(plots_dir, "latency_histogram.png"),
  plot = p_hist,
  width = 7,
  height = 5,
  dpi = 300
)

# Density Curve

p_density <- ggplot(timing, aes(x = dt_seconds)) +
  geom_density(linewidth = 1) +
  labs(
    title = "Latency Density Curve",
    x = "Anticipation Time (s)"
  ) +
  theme_minimal()

ggsave(
  file.path(plots_dir, "latency_density.png"),
  p_density,
  width = 7,
  height = 5,
  dpi = 300
)

# Boxplot per Object

p_box <- ggplot(timing, aes(x = hit_object, y = dt_seconds)) +
  geom_boxplot() +
  labs(
    title = "Anticipation Latency per Object",
    x = "Object",
    y = "Seconds before hit"
  ) +
  theme_minimal()

ggsave(
  file.path(plots_dir, "latency_boxplot_by_object.png"),
  p_box,
  width = 7,
  height = 5,
  dpi = 300
)

# Timeline Scatter (Inference vs Hit Time)

p_time <- ggplot(timing, aes(x = inf_time, y = dt_seconds)) +
  geom_point(alpha = 0.6) +
  labs(
    title = "Latency Over Time",
    x = "Inference Timestamp",
    y = "Seconds before hit"
  ) +
  theme_minimal()

ggsave(
  file.path(plots_dir, "latency_over_time.png"),
  p_time,
  width = 8,
  height = 5,
  dpi = 300
)


# ECDF Plot

p_ecdf <- ggplot(timing, aes(x = dt_seconds)) +
  stat_ecdf() +
  labs(
    title = "Cumulative Distribution of Anticipation Latency",
    x = "Seconds before hit",
    y = "Proportion of predictions"
  ) +
  theme_minimal()

ggsave(
  file.path(plots_dir, "latency_ecdf.png"),
  p_ecdf,
  width = 7,
  height = 5,
  dpi = 300
)

# Outlier Diagnostic Plot

if ("robust_z" %in% names(timing)) {
  
  p_outlier <- ggplot(timing, aes(x = dt_seconds, y = robust_z)) +
    geom_point() +
    geom_hline(yintercept = c(-3,3), linetype="dashed") +
    labs(
      title = "Outlier Diagnostics (Robust Z-score)",
      x = "Latency (s)",
      y = "Robust Z"
    ) +
    theme_minimal()
  
  ggsave(
    file.path(plots_dir, "latency_outliers.png"),
    p_outlier,
    width = 7,
    height = 5,
    dpi = 300
  )
}

