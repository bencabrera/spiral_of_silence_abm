graph [
	alpha 1.000000
	bot_attachment_method "PreferentialAttachment(3,1)"
	bot_influence 1.000000
	directed 0
	expression_threshold_init_method "Unif(0,1)"
	inner_confidence_init_method "Unif(0,1)"
	n_clusters 1
	network_model "Handmade"

	node [ 
		id 0 
		valence red
		expression_threshold 0.6
		inner_confidence 1
	]
	node [ 
		id 1 
		valence green
		expression_threshold 0.1
		inner_confidence 1.5
	]
	node [ 
		id 2 
		valence red
		expression_threshold 0.1
		inner_confidence 1.5
	]
	node [ 
		id 3 
		valence red
		expression_threshold 0.1
		inner_confidence 1.5
	]
	node [ 
		id 4 
		valence red
		expression_threshold 0.1
		inner_confidence 1.5
	]
	node [ 
		id 5 
		valence green
		expression_threshold 0.8
		inner_confidence 1.5
	]

	edge [ source 1 target 0 ]
	edge [ source 2 target 0 ]
	edge [ source 3 target 0 ]
	edge [ source 4 target 0 ]
	edge [ source 3 target 5 ]
	edge [ source 4 target 5 ]
]
