# Load necessary packages
library(dplyr)
library(tidyr)
library(ggplot2)
library(psych)
library(effsize)
library(rstatix)
library(coin)
library(extrafont)

font_import()
loadfonts(device="all")

# List available Windows fonts
fonts <- windowsFonts()
#print(names(fonts))

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
data <- read.csv("../../data/subjective-metrics/likert-scale-questionnaire.csv")

# Calculate Cronbach's Alpha for the dependent variables
cronbach_data <- data %>%
  select(starts_with("I.felt.in.control.during.the.task."),
         starts_with("The.robot.seemed.to.understand.my.actions."),
         starts_with("The.robot.s.behaviour.was.predictable."),
         starts_with("The.robot.helped.me.complete.the.task.more.efficiently."),
         starts_with("I.would.prefer.to.perform.the.task.with.the.robot.rather.than.alone."),
         starts_with("I.trusted.the.robot.s.choices.during.the.task."),
         starts_with("I.would.like.to.work.with.this.robot.on.other.tasks."),
         starts_with("I.felt.the.robot.was.competing.with.me.at.some.points.during.the.task."))

cronbach_alpha <- psych::alpha(cronbach_data, check.keys = TRUE)
print(cronbach_alpha$total$raw_alpha)

# Load the data
data <- read.csv("../../data/subjective-metrics/likert-scale-questionnaire-avg.csv")

# Update the condition labels
data$Condition <- ifelse(data$Condition == "Anticipation", "Anticipatory", 
                         ifelse(data$Condition == "No anticipation", "Non-anticipatory", 
                                data$Condition))

# Reshape data for visualization
data_long <- data %>%
  pivot_longer(cols = starts_with("Score"),
               names_to = "Score_Type", values_to = "Score")

# Perform normality tests and paired sample t-tests for each score type
ant_data <- data %>% filter(Condition == "Anticipatory") %>% pull("Score")
no_ant_data <- data %>% filter(Condition == "Non-anticipatory") %>% pull("Score")

# Perform Shapiro-Wilk normality test
shapiro_ant <- shapiro.test(ant_data)
shapiro_no_ant <- shapiro.test(no_ant_data)

# Print results with appropriate variable names
print(paste("Shapiro-Wilk normality test for Anticipatory condition:"))
print(shapiro_ant)

print(paste("Shapiro-Wilk normality test for Non-anticipatory condition:"))
print(shapiro_no_ant)

# Perform paired sample t-test if data is normally distributed, otherwise Wilcoxon signed-rank test
if (shapiro_ant$p.value > 0.05 & shapiro_no_ant$p.value > 0.05) {
  t_test_result <- t.test(ant_data, no_ant_data, paired = TRUE)
  print("Paired t-test for Anticipatory vs Non-anticipatory condition:")
  print(t_test_result)
  
  cohen_d_result <- effsize::cohen.d(ant_data, no_ant_data, paired = TRUE)
  print("Cohen's d effect size for Paired t-test:")
  print(cohen_d_result)
} else {
  wilcox_test_result <- wilcox.test(ant_data, no_ant_data, paired = TRUE)
  print("Wilcoxon signed-rank test for Anticipatory vs Non-anticipatory condition:")
  print(wilcox_test_result)
  
  # Calculate effect size using wilcox_effsize()
  wilcox_effsize_result <- wilcox_effsize(data_long, formula = Score ~ Condition, paired = TRUE)
  print("Effect size for Wilcoxon signed-rank test:")
  print(wilcox_effsize_result)
}

# Plot the data
plot_data <- data_long %>% filter(grepl("Score", Score_Type))

p <- ggplot(plot_data, 
            aes(x = Condition, 
                y = Score, 
                fill = Condition)
) +
  geom_bar(stat = "summary", 
           fun = "mean", 
           position = "dodge") +
  geom_errorbar(stat = "summary", 
                fun.data = "mean_se", 
                position = position_dodge(0.9), width = 0.25) +
  labs(x = "",
       y = "Mean scores") +
  theme_gray() +
  theme(
    axis.title = element_text(size = 20, face = "bold"),
    legend.title = element_text(size = 20, face = "bold"),
    legend.position = "none",  
    legend.justification = c("right", "top"),
    legend.background = element_rect(fill = "white"),
    text = element_text(family = "Garamond", size = 22),
    panel.grid = element_line(color = "gray"),
    panel.background = element_rect(fill = "white")
  ) +
  scale_y_continuous(breaks = seq(1, 5, by=1.0),
                     limits = c(1, 5),
                     oob = scales::squish
  ) +
  scale_fill_manual(labels = c("Anticipatory", "Non-anticipatory"), 
                    values = c("#4682B4", "#CD853F")
  ) +
  geom_segment(aes(x = 1.375, xend = 1.625, y = 4.75, yend = 4.75), size = 1.2) +
  annotate("text", x = 1.5, y = 4.85, label = "**", size = 8)

print(p)
ggsave(filename = paste0("./plots/likert-scale-bar-chart.jpg"), plot = p, width = 8, height = 6)
ggsave(
  filename = "./plots/likert-scale-bar-chart.pdf", 
  plot = p, 
  width = 8, 
  height = 4
)
