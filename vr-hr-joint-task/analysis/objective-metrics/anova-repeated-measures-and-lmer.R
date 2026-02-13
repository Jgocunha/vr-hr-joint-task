# Load necessary libraries
library(dplyr)
library(tidyr)
library(afex)


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

# Reshape data to long format for idle time
idle_long <- data %>%
  pivot_longer(cols = c(robot_idle_time, human_idle_time), 
               names_to = "type", 
               values_to = "idle_time") %>%
  mutate(type = ifelse(type == "robot_idle_time", "robot", "human"))

lmer_result <- lmer(idle_time ~ type * Condition + # Fixed effects
               (1 + Condition + type | ID), #, # Random effects, nested within subject
              data=idle_long, REML=TRUE) # Specifying data and estimation
summary(lmer_result)
anova(lmer_result)