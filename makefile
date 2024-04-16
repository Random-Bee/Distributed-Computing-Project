FOLDER_REPORT_FINAL = ./final_evaluation
FOLDER_REPORT_MID_TERM = ./mid_term_evaluation
FOLDER_SRC = ./src

all: docs compile

compile:
	$(MAKE) -C $(FOLDER_SRC)

docs: 
	$(MAKE) -C $(FOLDER_REPORT_FINAL)
	# $(MAKE) -C $(FOLDER_REPORT_MID_TERM)

clean:
	$(MAKE) -C $(FOLDER_REPORT_FINAL) clean
	$(MAKE) -C $(FOLDER_REPORT_MID_TERM) clean