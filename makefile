FOLDER_REPORT_FINAL = ./final_evaluation
FOLDER_REPORT_MID_TERM = ./mid_term_evaluation
FOLDER_SRC = ./src
TEST_SRC = ./test
BIN_FILE = ./bin/main
NODES = 60
M1=127.0.0.1

all: compile run

run: $(BIN_FILE)
	touch ./bin/inp.txt; echo ${NODES} > ./bin/inp.txt;
	echo ${M1} >> ./bin/inp.txt;
	cd ./bin; ./main 0

compile:
	$(MAKE) -C $(FOLDER_SRC)

compile_test:
	$(MAKE) -C $(TEST_SRC)

# Take number of nodes as input
test: compile_test
	python data.py
	cp ./inp-data.txt ./bin/inp-data.txt
	touch ./bin/inp.txt; echo ${NODES} > ./bin/inp.txt;
	echo ${M1} >> ./bin/inp.txt;
	cd ./bin; ./main 0

docs: 
	$(MAKE) -C $(FOLDER_REPORT_FINAL)
	# $(MAKE) -C $(FOLDER_REPORT_MID_TERM)

clean:
	$(MAKE) -C $(FOLDER_REPORT_FINAL) clean
	$(MAKE) -C $(FOLDER_REPORT_MID_TERM) clean
	$(MAKE) -C $(FOLDER_SRC) clean