FOLDER_REPORT_FINAL = ./final_evaluation
FOLDER_REPORT_MID_TERM = ./mid_term_evaluation

docs: 
	$(MAKE) -C $(FOLDER_REPORT_FINAL)
	# $(MAKE) -C $(FOLDER_REPORT_MID_TERM)

clean:
	$(MAKE) -C $(FOLDER_REPORT_FINAL) clean
	$(MAKE) -C $(FOLDER_REPORT_MID_TERM) clean