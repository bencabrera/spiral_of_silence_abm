#!/usr/bin/env Rscript

library(tidyr)
library(ggplot2)
library(data.table)
library(plyr)
library(dplyr)

# Ticks until stable by m
plot_ticks_until_stable_by_m <- function(data) {
	ggplot(data,aes(x = m, y = ticks_until_stable, group=m)) +
		geom_boxplot(outlier.shape = NA) +
		scale_y_continuous(name = "Ticks until stable",limits=c(0,1000)) +
		scale_x_continuous(breaks=seq(1,10)) +
		theme_bw()

	ggsave("ticks_until_stable_by_m.pdf",width=10,height=6.5)
}

# Opinion distribution at convergence by m
plot_opinion_dist_by_m <- function(data) {
	data_small <- data[,c("m","n_speaking_green","n_silenced_green","n_speaking_red","n_silenced_red")]
	data_small$n_silenced <- data_small$n_silenced_green+data_small$n_silenced_red
	data_small$percent_green <- data_small$n_speaking_green / (data_small$n_speaking_green+data_small$n_speaking_red+data_small$n_silenced)
	data_small$percent_red <- data_small$n_speaking_red / (data_small$n_speaking_green+data_small$n_speaking_red+data_small$n_silenced)
	data_small$percent_gray <- data_small$n_silenced / (data_small$n_speaking_green+data_small$n_speaking_red+data_small$n_silenced)

	tmp <- gather(data_small, key=color, value=percentage, percent_green:percent_gray)
	tmp$color <- substring(tmp$color,9)

	data_small <- as.data.table(tmp)
	
	tmp2 <- data_small[,.(mean_percent=mean(percentage)), by=list(m,color)]

	ggplot(tmp2,aes(x = m, y = mean_percent, fill = color)) +
		geom_col() +
		scale_y_continuous(name = "Percentage of individuals") +
		scale_fill_manual(values = c("dimgray", "forestgreen", "firebrick")) +
		labs(title = "Opinion distribution at convergence by m") +
		theme_bw()
	ggsave("opinion_by_m.pdf",width=10,height=6.5)
}

# Accuracy distribution by m
plot_accuracy_by_m <- function(data) {
	tmp <- data[,c("m","percent_accuracy_below_50", "percent_accuracy_50_to_60", "percent_accuracy_60_to_70", "percent_accuracy_70_to_80", "percent_accuracy_80_to_90", "percent_accuracy_above_90")]

	tmp <- gather(tmp, key = "accuracy_range", value = "percentage", percent_accuracy_below_50:percent_accuracy_above_90)
	tmp <- aggregate(percentage ~ m + accuracy_range,data = tmp,mean)

	# re-order levels (default is alphabetical)
	tmp$accuracy_range <- factor(tmp$accuracy_range, levels = c("percent_accuracy_above_90", "percent_accuracy_80_to_90", "percent_accuracy_70_to_80", "percent_accuracy_60_to_70", "percent_accuracy_50_to_60", "percent_accuracy_below_50"))

	ggplot(tmp,aes(x = m, y = percentage, fill = accuracy_range)) +
		geom_col() +
		scale_y_continuous(name = "Percentage of individuals") +
		scale_fill_brewer(name= "Accuracy", type = "seq", labels = c(".9+", ".8-.9", ".7-.8", ".6-.7", ".5-.6", ".0-0.5")) + 
		labs(title = "Accuracy distribution by m") +
		theme_bw()
	ggsave("accuracy_by_m.pdf",width=10,height=6.5)
}

# Majority / Minority distribution by m
plot_minority_majority_dist_by_m <- function(data) {
	data_small <- data[,c("m","n_speaking_green","n_silenced_green","n_speaking_red","n_silenced_red")]
	data_small$n_silenced <- data_small$n_silenced_green+data_small$n_silenced_red
	data_small$percent_green <- data_small$n_speaking_green / (data_small$n_speaking_green+data_small$n_speaking_red+data_small$n_silenced)
	data_small$percent_red <- data_small$n_speaking_red / (data_small$n_speaking_green+data_small$n_speaking_red+data_small$n_silenced)
	data_small$percent_gray <- data_small$n_silenced / (data_small$n_speaking_green+data_small$n_speaking_red+data_small$n_silenced)

	data_small$percent_majority <- apply(cbind(data_small$percent_green, data_small$percent_red), 1, max)
	data_small$percent_minority <- apply(cbind(data_small$percent_green, data_small$percent_red), 1, min)

	tmp <- gather(data_small, key = "group", value = "percentage",	percent_majority, percent_minority, percent_gray)
	tmp$group <- substring(tmp$group,9)
	tmp <- aggregate(percentage ~ m + group, data = tmp, mean)
	# re-order levels (default is alphabetical)
	tmp$group <- factor(tmp$group,levels = c("majority", "gray", "minority"))

	ggplot(tmp,aes(x = m, y = percentage, fill = group)) +
		geom_col() +
		scale_y_continuous(name = "Percentage of individuals") +
		scale_x_continuous(breaks=seq(1,10)) +
		scale_fill_brewer(type = "div") +
		labs(title = "Normalised opinion distribution at convergence by m") +
		theme_classic()

	ggsave("minority_majority_dist_by_m.pdf",width=10,height=6.5)
}

# Initial colour of central node vs. final colour of majority
plot_central_node_influence_by_m <- function(data) {
	data_small <- data[,c("m","n_speaking_green","n_speaking_red","most_central_valence")]
	data_small$winner <- ifelse(data_small$n_speaking_green > data_small$n_speaking_red, "green", "red")
	data_small$central_equal_majority <- ifelse(data_small$winner == data_small$most_central_valence, "yes", "no")

	ggplot(data_small,aes(x = m, fill = central_equal_majority)) +
		geom_bar() +
		scale_fill_discrete(name = "Central = Majority?") + 
		scale_x_continuous(breaks=seq(1,10)) +
		labs(title = "Initial colour of central node vs. final colour of majority (1)",subtitle = "Is the colour of the most central node in the beginning the same as the majority colour in the end?") +
		theme_classic()
	ggsave("central_node_influence_by_m.pdf",width=10,height=6.5)
}
