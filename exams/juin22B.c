mutex m;
cond_t  worker, mover, builder;
int builder_count = 0, worker_count = 0, mover_count = 0;


acquire_builder(){
	lock(m);
	while(builder_count + worker_count + mover_count >= 5 && builder_count < 1){
		wait(builder, m);  // na isxyei h syn8hkh kai gia builder
	}
	builder_count++;
	broadcast(mover);
	broadcast(worker);
	broadcast(builder);
	unlock(m);	
}

release_builder(){
	lock(m);
	builder_count--;
	broadcast(mover);
	broadcast(worker);
	broadcast(builder);
	unlock(m);
}

acquire_worker(){
	lock(m);
	while(builder_count + worker_count + mover_count >= 5 && builder_count < 2){
		wait(worker, m);
	}
	worker_count++;
	unlock(m);
}

release_worker(){
	lock(m);
	worker_count--;
	broadcast(mover);
	broadcast(worker);
	broadcast(builder);
	unlock(m);
}


acquire_mover(){
	lock(m);
	while(builder_count < 2){
		wait(mover, m);
	}
	mover_count++;
	unlock(m);
}

release_mover(){
	lock(m);
	mover_count--;
	broadcast(mover);
	broadcast(worker);
	broadcast(builder);
	unlock(m);
}