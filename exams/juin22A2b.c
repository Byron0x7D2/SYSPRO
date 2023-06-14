void fun(fd){
	uint32_t n1;
	int32_t n2;
	int rc = 0;
	char *data = (char *) &n1;
	int left = sizeof(n1);
	do{
		rc = read(fd, data, left);
		if(rc < 0) return;
		data += rc;
		left -= rc;
	}while(left);

	n1 = ntohl(n1);

	char buf[10];
	left = sizeof(buf);
	do{
		rc = read(fd, buf+left, left);
		if(rc < 0) return;
		left -= rc;
	}while(left);
	
	data = (char *) &n2;
	left = sizeof(n2);
	do{
		rc = read(fd, data, left);
		if(rc < 0) return;
		data += rc;
		left -= rc;
	}while(left);

	n2 = ntohl(n2);

	int32_t min = n1 < n2 ? n1 : n2;

	for(int32_t i = 0; i < min; i++){
		printf("%s", buf);
	}

}