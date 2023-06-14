void fun(fd){
	uint16_t n;
	int rc = 0;
	char *data = (char *) &n;
	int left = sizeof(n);
	do{
		rc = read(fd, data, left);
		if(rc < 0) return;
		data += rc;
		left -= rc;
	}while(left);

	n = ntohs(n);

	char buf[n+1];
	left = sizeof(buf) - 1;
	do{
		rc = read(fd, buf+left, left);
		if(rc < 0) return;
		left -= rc;
	}while(left);
}