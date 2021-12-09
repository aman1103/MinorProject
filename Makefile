build:
	chmod +x compile.sh
	./compile.sh

csv:
	python3 generate_csv.py

report:
	python3 csv_to_graph.py