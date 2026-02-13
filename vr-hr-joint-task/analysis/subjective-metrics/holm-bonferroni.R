
# Quick one-liner (all 4 subjective outcomes as one family)

p <- c(Competence = 0.001,
       Collaboration = 0.002,
       Discomfort = 0.009,
       Warmth = 0.032)

p_holm <- p.adjust(p, method = "holm")
p_bonf <- p.adjust(p, method = "bonferroni")

p_holm
# Interpretation: all Holm-adjusted p’s are < .05, so they remain significant.

# If you want RoSAS as one family and Collaboration separate
p_rosas <- c(Competence = 0.001, Discomfort = 0.009, Warmth = 0.032)
p_rosas_holm <- p.adjust(p_rosas, method = "holm")
p_collab <- 0.002  # reported unadjusted if treated as a priori separate
p_rosas_holm

# Tidyverse-friendly workflow (useful if you’re computing p’s in code)
library(dplyr)

df <- tibble::tibble(
  measure = c("Competence", "Collaboration", "Discomfort", "Warmth"),
  p = c(0.001, 0.002, 0.009, 0.032),
  family = "Subjective"  # single family; or set "RoSAS" vs "Collaboration"
)

df %>%
  group_by(family) %>%
  mutate(p_holm = p.adjust(p, method = "holm"),
         p_bonf = p.adjust(p, method = "bonferroni")) %>%
  ungroup()