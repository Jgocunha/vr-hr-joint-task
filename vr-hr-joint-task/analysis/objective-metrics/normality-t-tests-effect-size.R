# Install and load necessary packages
library(dplyr)
library(tidyr)
library(ggplot2)
library(extrafont)
library(ggsignif)
library(moments)
library(Hmisc)
library(effsize)
library(rstatix)
library(coin)

# Get the script directory
if (interactive()) {
  script_directory <- rstudioapi::getActiveDocumentContext()$path
  script_directory <- dirname(script_directory)
} else {
  script_directory <- getwd()  # Fallback to current working directory
}
setwd(script_directory)

# Load the data
current_directory <- getwd()
data <- read.csv("../../data/objective-metrics/data.csv")

# Read the data from CSV file
#data <- read_csv("C:/Users/Joao Gaspar/OneDrive - Universidade do Minho (1)/phd-degree/tue-hti-work/hrc-vr-project/data/objective-metrics/data.csv")


# Summarize data to calculate mean and standard error for all dependent variables by Condition
data_summary <- data %>%
  group_by(Condition) %>%
  summarise(
    mean_robot_target_count = mean(robot_target_count, na.rm = TRUE),
    se_robot_target_count = sd(robot_target_count, na.rm = TRUE) / sqrt(n()),
    mean_robot_grasp_count = mean(robot_grasp_count, na.rm = TRUE),
    se_robot_grasp_count = sd(robot_grasp_count, na.rm = TRUE) / sqrt(n()),
    mean_robot_place_count = mean(robot_place_count, na.rm = TRUE),
    se_robot_place_count = sd(robot_place_count, na.rm = TRUE) / sqrt(n()),
    mean_human_grasp_count = mean(human_grasp_count, na.rm = TRUE),
    se_human_grasp_count = sd(human_grasp_count, na.rm = TRUE) / sqrt(n()),
    mean_human_place_count = mean(human_place_count, na.rm = TRUE),
    se_human_place_count = sd(human_place_count, na.rm = TRUE) / sqrt(n()),
    mean_planning_count = mean(planning_count, na.rm = TRUE),
    se_planning_count = sd(planning_count, na.rm = TRUE) / sqrt(n()),
    mean_replanning_count = mean(replanning_count, na.rm = TRUE),
    se_replanning_count = sd(replanning_count, na.rm = TRUE) / sqrt(n()),
    mean_human_idle_time = mean(human_idle_time, na.rm = TRUE),
    se_human_idle_time = sd(human_idle_time, na.rm = TRUE) / sqrt(n()),
    mean_robot_idle_time = mean(robot_idle_time, na.rm = TRUE),
    se_robot_idle_time = sd(robot_idle_time, na.rm = TRUE) / sqrt(n()),
    mean_collision_count = mean(collision_count, na.rm = TRUE),
    se_collision_count = sd(collision_count, na.rm = TRUE) / sqrt(n()),
    mean_common_grasp_count = mean(common_grasp_count, na.rm = TRUE),
    se_common_grasp_count = sd(common_grasp_count, na.rm = TRUE) / sqrt(n()),
    mean_task_duration = mean(task_duration, na.rm = TRUE),
    se_task_duration = sd(task_duration, na.rm = TRUE) / sqrt(n()),
    .groups = 'drop'
  )

# Filter data for only "anticipation" and "no anticipation" conditions
filtered_data <- data %>% 
  filter(Condition %in% c("anticipation", "no anticipation"))

# Function to perform normality test, appropriate t-test or Wilcoxon test, and calculate effect size
perform_tests <- function(variable) {
  anticipation_data <- filtered_data %>% filter(Condition == "anticipation") %>% pull(variable)
  no_anticipation_data <- filtered_data %>% filter(Condition == "no anticipation") %>% pull(variable)
  
  # Perform Shapiro-Wilk normality test
  shapiro_anticipation <- shapiro.test(anticipation_data)
  shapiro_no_anticipation <- shapiro.test(no_anticipation_data)
  
  test_result <- list()
  
  # Check normality
  if (shapiro_anticipation$p.value > 0.05 & shapiro_no_anticipation$p.value > 0.05) {
    # Perform paired t-test
    t_test_result <- t.test(anticipation_data, no_anticipation_data, paired = TRUE)
    cohen_d_result <- cohen.d(anticipation_data, no_anticipation_data, paired = TRUE)
    test_result <- list(
      test = "Paired t-test",
      t_or_v_value = t_test_result$t.value,
      p_value = t_test_result$p.value,
      effect_size = cohen_d_result$estimate,
      effect_size_type = "cohen_d"
    )
  } else {
    # Perform Wilcoxon signed-rank test
    wilcox_test_result <- wilcox.test(anticipation_data, no_anticipation_data, paired = TRUE)
    wilcox_effsize_result <- wilcox_effsize(filtered_data, formula = as.formula(paste(variable, "~ Condition")), paired = TRUE)
    test_result <- list(
      test = "Wilcoxon signed-rank test",
      t_or_v_value = wilcox_test_result$statistic,
      p_value = wilcox_test_result$p.value,
      effect_size = wilcox_effsize_result$effsize,
      effect_size_type = "wilcox_effsize"
    )
  }
  
  return(c(
    variable,
    shapiro_anticipation$statistic,
    shapiro_anticipation$p.value,
    shapiro_no_anticipation$statistic,
    shapiro_no_anticipation$p.value,
    test_result$test,
    test_result$t_or_v_value,
    test_result$p_value,
    test_result$effect_size,
    test_result$effect_size_type
  ))
}

# List of dependent variables
dependent_variables <- c("robot_target_count", "robot_grasp_count", "robot_place_count",
                         "human_grasp_count", "human_place_count", "planning_count",
                         "replanning_count", "human_idle_time", "robot_idle_time",
                         "collision_count", "common_grasp_count", "task_duration")

# Perform tests for all dependent variables
results <- t(sapply(dependent_variables, perform_tests))

# Create a summary data frame
results_summary <- data.frame(
  Variable = results[,1],
  Shapiro_Anticipation_W = as.numeric(results[,2]),
  Shapiro_Anticipation_p = as.numeric(results[,3]),
  Shapiro_No_Anticipation_W = as.numeric(results[,4]),
  Shapiro_No_Anticipation_p = as.numeric(results[,5]),
  Test = results[,6],
  t_or_v_value = as.numeric(results[,7]),
  p_value = as.numeric(results[,8]),
  effect_size = as.numeric(results[,9]),
  effect_size_type = results[,10]
)

# Display the summary
print(results_summary)
write.csv(results_summary, 'results.csv')
