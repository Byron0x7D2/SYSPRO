mutex m;
cond talker, listener;
int num_talkers = 0, num_listeners = 0;

acquire_talker(){
	lock(m);
	while(num_talkers > 2){
		wait(talker, m);
	}
	num_talkers++;
	unlock(m);
}

release_talker(){
	lock(m);
	num_talkers--;
	signal(talker);
	unlock(m);
}

acquire_listener(){
	lock(m);
	while(num_listeners > 3){
		wait(listener, m);
	}
	num_listeners++;
	unlock(m);
}

release_listener(){
	lock(m);
	num_listeners--;
	signal(talker);
	unlock(m);
}