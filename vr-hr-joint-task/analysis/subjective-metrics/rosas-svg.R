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

#font_import()
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

# Load the data
data <- readxl::read_xlsx("../../data/subjective-metrics/rosas-questionnaire.xlsx")

# Calculate Cronbach's Alpha for the dependent variables
cronbach_alpha_competence <- psych::alpha(data[, c("C1", "C2", "C3", "C4","C5", "C6")], check.keys = TRUE)
print(cronbach_alpha_competence$total$raw_alpha)

cronbach_alpha_discomfort <- psych::alpha(data[, c("D1", "D2", "D3", "D4","D5", "D6")], check.keys = TRUE)
print(cronbach_alpha_discomfort$total$raw_alpha)

cronbach_alpha_warmth <- psych::alpha(data[, c("W1", "W2", "W3", "W4","W5", "W6")], check.keys = TRUE)
print(cronbach_alpha_warmth$total$raw_alpha)

# Calculating average scores for Competence, Discomfort, and Warmth
data$Competence <- rowMeans(data[, c("C1", "C2", "C3", "C4","C5", "C6")])
data$Discomfort <- rowMeans(data[, c("D1", "D2", "D3", "D4","D5", "D6")])
data$Warmth <- rowMeans(data[, c("W1", "W2", "W3", "W4","W5", "W6")])

# Reshape data for visualization
data_long <- data %>%
  pivot_longer(cols = c("Competence", "Discomfort", "Warmth"), 
               names_to = "Score_Type", 
               values_to = "Score") 

# Significance annotations *, **, *** 
add_manual_significance <- function(p) {
  p +
    # Add horizontal lines
    geom_segment(aes(x = 0.875, xend = 1.125, y = 4.75, yend = 4.75), size = 1.2) +
    geom_segment(aes(x = 1.875, xend = 2.125, y = 4.75, yend = 4.75), size = 1.2) +
    geom_segment(aes(x = 2.875, xend = 3.125, y = 4.75, yend = 4.75), size = 1.2) +
    
    # Add significance text with complete control over size
    annotate("text", x = 1, y = 4.85, label = "**", size = 8) +
    annotate("text", x = 2, y = 4.85, label = "**", size = 8) +
    annotate("text", x = 3, y = 4.85, label = "*", size = 8)
}

# Plot the data
bar_chart_plot <- ggplot(data_long, 
                         aes(x = Score_Type, y = Score, fill = Condition)) +
  geom_bar(stat = "summary", 
           fun = "mean", 
           position = "dodge") +
  geom_errorbar(stat = "summary", 
                #fun.args = list(mult = 1), 
                fun.data = "mean_se", 
                position = position_dodge(0.9), width = 0.25) +
  labs(
       x = "Social attributes",
       y = "Mean scores") +
  theme_gray() +
  theme(
    axis.title = element_text(size = 20, face = "bold"),
    legend.title = element_text(size = 20, face = "bold"),
    legend.position = "inside",  
    legend.justification = c("right"),
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
                    ) 

# Add the significance annotations
bar_chart_plot <- add_manual_significance(bar_chart_plot)
ggsave(filename = paste0("./plots/rosas-bar-chart-per-condition.jpg"), plot = bar_chart_plot, width = 8, height = 6)
print(bar_chart_plot)
ggsave(
  filename = "./plots/rosas-bar-chart-per-condition.pdf", 
  plot = bar_chart_plot, 
  width = 8, 
  height = 4
)

# Normality testing
shapiro_competence_ant <- shapiro.test(data %>% filter(Condition == "Anticipation") %>% pull(Competence))
shapiro_competence_no_ant <- shapiro.test(data %>% filter(Condition == "No anticipation") %>% pull(Competence))

shapiro_warmth_ant <- shapiro.test(data %>% filter(Condition == "Anticipation") %>% pull(Warmth))
shapiro_warmth_no_ant <- shapiro.test(data %>% filter(Condition == "No anticipation") %>% pull(Warmth))

shapiro_discomfort_ant <- shapiro.test(data %>% filter(Condition == "Anticipation") %>% pull(Discomfort))
shapiro_discomfort_no_ant <- shapiro.test(data %>% filter(Condition == "No anticipation") %>% pull(Discomfort))

# Print Shapiro-Wilk test results
print(shapiro_competence_ant)
print(shapiro_competence_no_ant)
print(shapiro_warmth_ant)
print(shapiro_warmth_no_ant)
print(shapiro_discomfort_ant)
print(shapiro_discomfort_no_ant)

# Prepare data for paired t-tests or wilcoxon rank test
competence_ant = data %>% filter(Condition == "Anticipation") %>% pull(Competence)
competence_no_ant = data %>% filter(Condition == "No anticipation") %>% pull(Competence)
warmth_ant = data %>% filter(Condition == "Anticipation") %>% pull(Warmth)
warmth_no_ant = data %>% filter(Condition == "No anticipation") %>% pull(Warmth)
discomfort_ant = data %>% filter(Condition == "Anticipation") %>% pull(Discomfort)
discomfort_no_ant = data %>% filter(Condition == "No anticipation") %>% pull(Discomfort)

# Perform paired sample t-tests
competence_ttest <- t.test(competence_ant, competence_no_ant, paired = TRUE)
warmth_ttest <- t.test(warmth_ant, warmth_no_ant, paired = TRUE)
discomfort_ttest <- t.test(discomfort_ant, discomfort_no_ant, paired = TRUE)

# Print t-test results
print(competence_ttest)
#print(warmth_ttest)
#print(discomfort_ttest)

# Perform Wilcoxon signed-rank tests for Warmth and Discomfort (non-normally distributed)
warmth_wilcoxon <- wilcox.test(warmth_ant, warmth_no_ant, paired = TRUE , exact = FALSE)
discomfort_wilcoxon <- wilcox.test(discomfort_ant, discomfort_no_ant, paired = TRUE , exact = FALSE)

# Print test results
print(warmth_wilcoxon)
print(discomfort_wilcoxon)

# Calculate effect sizes
# Calculate Cohen's d for paired t-tests
cohen_d_competence <- effsize::cohen.d(competence_ant, competence_no_ant, paired = TRUE)
#cohen_d_warmth <- cohen.d(warmth_ant, warmth_no_ant, paired = TRUE)
#cohen_d_discomfort <- cohen.d(discomfort_ant, discomfort_no_ant, paired = TRUE)

print(cohen_d_competence)
#print(cohen_d_warmth)
#print(cohen_d_discomfort)

# Calculate Wilcoxon effect size using rstatix
warmth_effect_size <- wilcox_effsize(data, Warmth ~ Condition, paired = TRUE)
discomfort_effect_size <- wilcox_effsize(data, Discomfort ~ Condition, paired = TRUE)

print(warmth_effect_size)
print(discomfort_effect_size)

