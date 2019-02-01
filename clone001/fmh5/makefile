all:
	@make -j8 -C save
	@make -j8 -C proto
	@make -C app
	@make -C cgiapp
clean:
	@make -C save clean
	@make -C proto clean
	@make -C app clean
	@make -C cgiapp clean
