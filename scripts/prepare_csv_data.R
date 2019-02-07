#!/usr/bin/env Rscript

library(tidyr)
library(ggplot2)
library(data.table)
library(stringr)

basic_read_and_prepare_data <- function(path) {
	data <- fread(path)
	return(data)
}

add_m_column_for_network_model <- function(data) {
	data$m <- as.integer(str_match(data$network_model, "BarabasiAlbert\\(([0-9]+)\\)")[,-1])	
	return(data)
}

# # convert results to correct format: transport, unlist each column, make data frame
# bot_simulation_results <- data.frame(apply(t(bot_simulation_results), 2, unlist))
# bot_simulation_results$m <- as.factor(bot_simulation_results$m)
# bot_simulation_results$bot_attachment_method <- as.factor(bot_simulation_results$bot_attachment_method)
# levels(bot_simulation_results$bot_attachment_method) <- c("Preferential attachment", "Inverse preferential attachment", "Random attachment")
# bot_simulation_results$color_most_central_before <- as.factor(bot_simulation_results$color_most_central_before)
# levels(bot_simulation_results$color_most_central_before) <- c("gray", "red", "green")
# bot_simulation_results$color_most_central_after <- as.factor(bot_simulation_results$color_most_central_after)
# levels(bot_simulation_results$color_most_central_after) <- c("gray", "red", "green")
# bot_simulation_results$opinion_most_central <- as.factor(bot_simulation_results$opinion_most_central)
# levels(bot_simulation_results$opinion_most_central) <- c("red", "green")
# bot_simulation_results$confidence_initialization_method <- as.factor(bot_simulation_results$confidence_initialization_method)
# levels(bot_simulation_results$confidence_initialization_method) <- c("uniform_0_1", "truncated_normal_50_50", "truncated_normal_0_50","exponential_1")
# bot_simulation_results$expression_threshold_initialization_method <- as.factor(bot_simulation_results$expression_threshold_initialization_method)
# levels(bot_simulation_results$expression_threshold_initialization_method) <- c("uniform_0_1", "truncated_normal_50_50", "truncated_normal_0_50","exponential_1")