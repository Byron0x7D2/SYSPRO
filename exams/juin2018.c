mutex m;
cond red, green, blue;
int nbRed = 0, nbGreen = 0, nbBlue = 0;

acquire_red(){
	lock(m);
	while(nbRed){
		wait(red, m);
	}
	nbRed++;
	unlock(m);
}

release_red(){
	lock(m);
	nbRed--;
	signal(red);
	unlock(m);
}

acquire_green(){
	lock(m);
	while(nbGreen){
		wait(green, m);
	}
	nbGreen++;
	unlock(m);
}

release_green(){
	lock(m);
	nbGreen--;
	signal(green);
	unlock(m);
}

acquire_blue(){
	lock(m);
	while(nbBlue){
		wait(blue, m);
	}
	nbBlue++;
	unlock(m);
}

release_blue(){
	lock(m);
	nbBlue--;
	signal(blue);
	unlock(m);
}