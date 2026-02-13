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

# font_import()
loadfonts(device="all")

# Filter data for only "anticipation" and "no anticipation" conditions
filtered_data <- data %>% 
  filter(Condition %in% c("anticipation", "no anticipation"))

# List of dependent variables
# Select the target variable here
dependent_variables <- c("task_duration")

# List of dependent variables with their respective descriptions
dependent_variables_labels <- c(
  #"human_idle_time" = "Time (s)"
  #"robot_idle_time" = "Time (s)"
  #"collision_count" = "Number of collisions"
  "task_duration" = "Time (s)"
)

# Function to create bar-chart plots for every variable with appropriate y-axis label
create_plots <- function(variable) {
  
  # Format the variable name before using it in pivot_longer
  x_label <- gsub("_", " ", variable) # Replace underscores with spaces
  x_label <- paste(toupper(substr(x_label, 1, 1)), substr(x_label, 2, nchar(x_label)), sep="") # Capitalize first letter
  
  data_variable <- filtered_data %>%
    pivot_longer(cols = variable,
                 names_to = "Score_Type", 
                 values_to = "Score") %>%
    # Replace the original column name with the formatted name
    mutate(Score_Type = x_label)
  
  # Calculate the maximum value for positioning the significance annotation
  # Use a simpler approach that works with older dplyr versions
  mean_values <- aggregate(Score ~ Condition, data = data_variable, FUN = mean, na.rm = TRUE)
  colnames(mean_values)[2] <- "mean_val"
  
  # Get the maximum mean value and add some margin for the annotation
  max_height <- max(mean_values$mean_val, na.rm = TRUE) * 1.2
  text_height <- max(mean_values$mean_val, na.rm = TRUE) * 1.25
  
  # Create a label for the y-axis based on the variable
  y_label <- dependent_variables_labels[variable]
  
  # Plot the data
  bar_chart_plot <- ggplot(data_variable, 
                           aes(x = Score_Type, y = Score, fill = Condition)) +
    geom_bar(stat = "summary", 
             fun = "mean", 
             position = position_dodge(0.7),
             width = 0.6) +
    geom_errorbar(stat = "summary", 
                  fun.data = "mean_se", 
                  position = position_dodge(0.7), width = 0.25) +
    labs(
      x = "", # Remove the x-axis label since it's now part of the data
      y = y_label
      #y = "" # no y-axis label
    ) +
    theme_gray() +
    theme(
      axis.title = element_text(size = 24, face = "bold"),
      legend.title = element_text(size = 24, face = "bold"),
      legend.position = "inside", # or "none" or "inside"
      legend.justification = c("right", "bottom"),
      legend.background = element_rect(fill = "white"),
      text = element_text(family = "Garamond", size = 26),
      panel.grid = element_line(color = "gray"),
      panel.background = element_rect(fill = "white")
    )  +
    scale_fill_manual(labels = c("Anticipatory", "Non-anticipatory"), 
                      values = c("#4682B4", "#CD853F")
    )
  
  # Add significance notation outside of aes()
  #bar_chart_plot <- bar_chart_plot +
  #  geom_segment(x = 0.9, xend = 1.1, y = max_height, yend = max_height, size = 1.2) +
  #  annotate("text", x = 1, y = text_height, label = "***", size = 8)
  
  print(bar_chart_plot)
  
  # Save the plot with a dynamic file name based on the variable
  directory = paste("./plots/", variable , ".svg")
  #ggsave(filename = directory, plot = bar_chart_plot, width = 5, height = 6)
  ggsave(
    filename = directory, 
    plot = bar_chart_plot, 
    width = 5, 
    height = 4
  )
  
}
# Apply the function to each dependent variable
t(sapply(dependent_variables, create_plots))
