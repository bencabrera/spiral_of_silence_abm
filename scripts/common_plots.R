#!/usr/bin/env Rscript

library(tidyr)
library(ggplot2)
library(data.table)
library(plyr)
library(dplyr)

# compute mean accuracy by and and gamma
compute_mean_accuracy_by_m_gamma <- function(data) {
	data_small <- data[,.(avg_accuracy,m,gamma)]
	tmp <- data_small[,.(mean_accuracy=mean(avg_accuracy)),by=.(m,gamma)]

	return(tmp)
}

# Ticks until stable by m
plot_ticks_until_stable_by_m <- function(data) {
	ggplot(data,aes(x = m, y = ticks_until_stable, group=m)) +
		geom_boxplot(outlier.shape = NA) +
		scale_y_continuous(name = "Ticks until stable",limits=c(0,50)) +
		scale_x_continuous() +
		theme_bw()

	ggsave("ticks_until_stable_by_m.pdf",width=10,height=6.5)
}

plot_ticks_until_stable_by_m_gamma <- function(data) {
	data$m <- as.factor(data$m)
	data$gamma <- as.factor(data$gamma)
	ggplot(data,aes(x = m, y = ticks_until_stable, color=gamma)) +
		geom_boxplot(outlier.shape = NA) +
		scale_y_continuous(name = "Ticks until stable",limits=c(0,1300)) +
		theme_bw()

	ggsave("ticks_until_stable_by_m_gamma.pdf",width=8,height=5)
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

# Accuracy distribution by m and gamma
plot_accuracy_by_m_gamma <- function(data) {
	tmp <- data[,c("m","gamma","percent_accuracy_below_50", "percent_accuracy_50_to_60", "percent_accuracy_60_to_70", "percent_accuracy_70_to_80", "percent_accuracy_80_to_90", "percent_accuracy_above_90")]

	tmp <- gather(tmp, key = "accuracy_range", value = "percentage", percent_accuracy_below_50:percent_accuracy_above_90)
	tmp <- aggregate(percentage ~ m + gamma + accuracy_range,data = tmp,mean)

	# re-order levels (default is alphabetical)
	tmp$accuracy_range <- factor(tmp$accuracy_range, levels = c("percent_accuracy_above_90", "percent_accuracy_80_to_90", "percent_accuracy_70_to_80", "percent_accuracy_60_to_70", "percent_accuracy_50_to_60", "percent_accuracy_below_50"))

	ggplot(tmp,aes(x = m, y = percentage, fill = accuracy_range)) +
		geom_col() +
		scale_y_continuous(name = "Percentage of individuals") +
		scale_fill_brewer(name= "Accuracy", type = "seq", labels = c(".9+", ".8-.9", ".7-.8", ".6-.7", ".5-.6", ".0-0.5")) + 
		labs(title = "Accuracy distribution by m") +
		theme_bw() +
		facet_wrap(~ gamma)
	ggsave("accuracy_by_m_gamma.pdf",width=10,height=6.5)
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
		scale_x_continuous() +
		scale_fill_brewer(type = "div") +
		labs(title = "Normalised opinion distribution at convergence by m") +
		theme_classic()

	ggsave("minority_majority_dist_by_m.pdf",width=10,height=6.5)
}

# Majority / Minority distribution by m and gamma
plot_minority_majority_dist_by_m_gamma <- function(data) {
	data_small <- data[,c("m","gamma","n_speaking_green","n_silenced_green","n_speaking_red","n_silenced_red")]
	data_small$n_silenced <- data_small$n_silenced_green+data_small$n_silenced_red
	data_small$percent_green <- data_small$n_speaking_green / (data_small$n_speaking_green+data_small$n_speaking_red+data_small$n_silenced)
	data_small$percent_red <- data_small$n_speaking_red / (data_small$n_speaking_green+data_small$n_speaking_red+data_small$n_silenced)
	data_small$percent_gray <- data_small$n_silenced / (data_small$n_speaking_green+data_small$n_speaking_red+data_small$n_silenced)

	data_small$percent_majority <- apply(cbind(data_small$percent_green, data_small$percent_red), 1, max)
	data_small$percent_minority <- apply(cbind(data_small$percent_green, data_small$percent_red), 1, min)

	tmp <- gather(data_small, key = "group", value = "percentage",	percent_majority, percent_minority, percent_gray)
	tmp$group <- substring(tmp$group,9)
	tmp <- aggregate(percentage ~ m + gamma + group, data = tmp, mean)
	# re-order levels (default is alphabetical)
	tmp$group <- factor(tmp$group,levels = c("majority", "gray", "minority"))

	ggplot(tmp,aes(x = m, y = percentage, fill = group)) +
		geom_col() +
		scale_y_continuous(name = "Percentage of individuals") +
		scale_x_continuous() +
		scale_fill_brewer(type = "div") +
		labs(title = "Normalised opinion distribution at convergence by m") +
		theme_classic() + 
		facet_wrap(~gamma)

	ggsave("minority_majority_dist_by_m_gamma.pdf",width=10,height=6.5)
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

# Initial colour of central node vs. final colour of majority
plot_central_node_influence_by_m_gamma <- function(data) {
	data_small <- data[,c("m","gamma","n_speaking_green","n_speaking_red","most_central_valence")]
	data_small$winner <- ifelse(data_small$n_speaking_green > data_small$n_speaking_red, "green", "red")
	data_small$central_equal_majority <- ifelse(data_small$winner == data_small$most_central_valence, "yes", "no")

	ggplot(data_small,aes(x = m, fill = central_equal_majority)) +
		geom_bar() +
		scale_fill_discrete(name = "Central = Majority?") + 
		scale_x_continuous(breaks=seq(1,10)) +
		labs(title = "Initial colour of central node vs. final colour of majority (1)",subtitle = "Is the colour of the most central node in the beginning the same as the majority colour in the end?") +
		theme_classic() +
		facet_wrap(~gamma)
	ggsave("central_node_influence_by_m_gamma.pdf",width=10,height=6.5)
}

# Win rate of red by bot attachment method
plot_win_rate_by_bot_method <- function(data) {
	data$red_wins <- ifelse(data$n_speaking_red > data$n_speaking_green, 1, 0)
	data$percent_bots <- data$n_bots / data$n_users

	data_small <- data[,c("m", "bot_attachment_method", "bot_influence", "percent_bots", "red_wins")]

	tmp <- data_small[,.(win_probability=mean(red_wins)), by=list(m,bot_attachment_method,bot_influence,percent_bots)]

	# IMPORTANT: here we restrict only to bot_influence == 1
	tmp <- tmp[bot_influence==1]
	tmp$m <- as.factor(tmp$m)

	ggplot(tmp,aes(x=percent_bots, y=win_probability, linetype=m)) +
		geom_smooth(se=TRUE, level = 0.95, colour = "#08519c", span = 0.75) +
		geom_hline(yintercept = 0.5, linetype = "dashed", colour = "grey50") +
		scale_x_continuous(name = "Bots added (%)", labels = function(x){x*100}) +
		scale_y_continuous(name = "Probability bot opinion dominates (%)", labels = function(x){x*100}) +
		facet_wrap(~bot_attachment_method, ncol = 3) +
		#       labs(title="Influence of Bots to Win Rate with Bot Influence Factor") +
		theme_classic()

	ggsave("win_rate_by_bot_method.pdf", width=9, height=3, dpi=300)
}

# Win rate of red by bot influence
plot_win_rate_by_bot_influence <- function(data) {
	data$red_wins <- ifelse(data$n_speaking_red > data$n_speaking_green, 1, 0)
	data$percent_bots <- data$n_bots / data$n_users

	data_small <- data[,c("m", "bot_attachment_method", "bot_influence", "percent_bots", "red_wins")]

	tmp <- data_small[,.(win_probability=mean(red_wins)), by=list(m,bot_attachment_method,bot_influence,percent_bots)]

	tmp$m <- as.factor(tmp$m)
	tmp <- tmp[bot_attachment_method=="BA"]
	tmp <- tmp[bot_influence %in% c(0.25,0.5,0.75)]

	ggplot(tmp,aes(x=percent_bots, y=win_probability, linetype=m)) +
		geom_smooth(se=TRUE, level = 0.95, colour = "#08519c", span = 0.75) +
		geom_hline(yintercept = 0.5, linetype = "dashed", colour = "grey50") +
		scale_x_continuous(name = "Bots added (%)", labels = function(x){x*100}) +
		scale_y_continuous(name = "Probability bot opinion dominates (%)", labels = function(x){x*100}) +
		facet_wrap(~bot_influence, ncol = 3) +
		theme_classic()

	ggsave("win_rate_by_bot_influence.pdf", width=9, height=3, dpi=300)
}


# --- SBM ---

# Distance between cluster winners by inter cluster density
plot_cluster_winners_by_inter_cluster_density <- function(data) {
	data$cluster_winner_distance <- abs(data$n_cluster_green_dominates-data$n_cluster_red_dominates)
	data_small <- data[,.(mean_cluster_winner_distance=mean(cluster_winner_distance)),by=inter_cluster_density]
	ggplot(data_small,aes(x = inter_cluster_density, y = mean_cluster_winner_distance)) +
		geom_line() +
		# scale_y_continuous(name = "Ticks until stable",limits=c(0,50)) +
		# scale_x_continuous() +
		theme_bw()

	ggsave("cluster_winners_by_inter_cluster_density.pdf",width=10,height=6.5)
}


plot_probability_to_win_by_how_many_central <- function(data) {
	data_small <- data[,.(m,gamma,n_speaking_green,n_speaking_red,n_top_degree_green,n_top_degree_red)]

	data_small$green_wins <- ifelse(data_small$n_speaking_green > data_small$n_speaking_red, 1, 0)

	# return(data_small)

	# compute mean grouped over all values of n_top_degree_green -> gives probability for green to win based on n_top_degree_green
	tmp <- data_small[,.(p_green_wins=mean(green_wins)),by=.(m,gamma,n_top_degree_green)]
	tmp <- tmp[m==5]

	tmp$n_top_degree_green <- as.integer(tmp$n_top_degree_green)

	ggplot(tmp,aes(x = n_top_degree_green, y = p_green_wins)) +
		geom_point() +
		geom_smooth() +
		theme_bw() +
		facet_wrap(~gamma)
	ggsave("probability_to_win_by_how_many_central.pdf",width=8,height=3)
}
