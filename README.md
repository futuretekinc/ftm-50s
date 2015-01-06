ftm-50s
=======

	1. Futuretek Github에 아이디 등록
	2. git clone 으로 프로젝트 다운로드

		*  https://등록된아이디@github.com/futuretekinc/ftm-50s.git

	3. kernel 빌드

		1. ftm-50s/linux-2.6.37/ 폴더 이동
		2. config 파일 copy ==>> cp config.ftm-50s .config
		3. make uImage
		4. 빌드완료후 arch/arm/boot/ 폴더 이동
		5. uImage 파일을 /tftpboot/ftm-50s-new/ 폴더 밑에 복사

	4. rootfs 빌드

		1. ftm-50s/rootfs/ 폴더 이동
		2. sudo ./build
		3. 빌드 완료된 rootfs.img 파일은 /tftpboot/ftm-50-new/ 폴더 밑에 자동복사

	5. u-boot 에서 실행

		1. ipaddr, serverip, gatewayip 를 설정
		2. run rf_kernel rf_rootfs 실행
		3. 완료후 reset


