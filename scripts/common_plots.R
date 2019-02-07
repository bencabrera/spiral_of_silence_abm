#!/usr/bin/env Rscript

library(tidyr)
library(ggplot2)
library(data.table)

# Ticks until stable by m
ticks_until_stable_by_m <- function(data) {
	ggplot(data,aes(x = m, y = ticks_until_stable, group=m)) +
		geom_boxplot(outlier.shape = NA) +
		scale_y_continuous(name = "Ticks until stable",limits=c(0,1000)) +
		scale_x_continuous(breaks=seq(1,10)) +
		theme_bw()

	ggsave("ticks_until_stable_by_m.pdf",width=10,height=6.5)
}
