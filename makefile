FOLDER_REPORT_FINAL = ./final_evaluation
FOLDER_REPORT_MID_TERM = ./mid_term_evaluation
FOLDER_SRC = ./src
BIN_FILE = ./bin/main
NODES = 60
M1=127.0.0.1

all: compile

run: $(BIN_FILE)
	touch ./bin/inp.txt; echo ${NODES} > ./bin/inp.txt;
	echo ${M1} >> ./bin/inp.txt;
	cd ./bin; ./main 0

compile:
	$(MAKE) -C $(FOLDER_SRC)

docs: 
	$(MAKE) -C $(FOLDER_REPORT_FINAL)
	# $(MAKE) -C $(FOLDER_REPORT_MID_TERM)

clean:
	$(MAKE) -C $(FOLDER_REPORT_FINAL) clean
	$(MAKE) -C $(FOLDER_REPORT_MID_TERM) clean
	$(MAKE) -C $(FOLDER_SRC) clean