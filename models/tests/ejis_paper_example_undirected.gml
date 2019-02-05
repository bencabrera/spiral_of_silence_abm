graph [
	directed 0
	bot_influence 0.8
	prop1 bla

	node [ 
		id 0 
		valence green
		expression_threshold 0.6
		inner_confidence 1.5
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

	edge [ source 1 target 0 ]
	edge [ source 2 target 0 ]
	edge [ source 3 target 0 ]
	edge [ source 4 target 0 ]
]
