all:
	@make -j8 -C ./common
	@make -j8 -C ./kernel
	@make -j8 -C ./platform
	@make -j8 -C ./data
	@make -j8 -C ./logic
	@make -j8 -C ./cgi
	@make -j8 -C ./test
clean:
	@make -C ./common clean
	@make -C ./kernel clean
	@make -C ./platform clean
	@make -C ./data clean
	@make -C ./logic clean
	@make -C ./cgi clean
	@make -C ./test clean
