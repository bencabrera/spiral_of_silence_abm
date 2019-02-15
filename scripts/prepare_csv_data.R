#!/usr/bin/env Rscript

library(tidyr)
library(ggplot2)
library(data.table)
library(stringr)

basic_read_and_prepare_data <- function(path) {
	data <- fread(path)

	data$most_central_valence <- ifelse(data$most_central_valence == 1, "red", "green")

	return(data)
}

add_m_gamma_column_for_network_model <- function(data) {
	extract <- str_match(data$network_model, "([0-9\\.]+),([-0-9\\.]+)")
	data$m <- as.integer(extract[,2])	
	data$gamma <- as.numeric(extract[,3])	
	data$gamma <- as.factor(data$gamma)
	return(data)
}

add_inter_cluster_density_for_sbm <- function(data) {
	extract <- str_match(data$network_model, "([0-9\\.]+)\\)$")
	data$inter_cluster_density <- as.numeric(extract[,2])
	return(data)
}

add_n_communities_for_sbm <- function(data) {
	data$n_communities <- unlist(lapply(data$n_speaking_green_by_cluster, function(x) { length(tstrsplit(x,",")) }))
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
