// use std::io;

use nix::unistd::*;

fn main() {
    println!("hello world (pid:{})\n", std::process::id());
    let mut shared_int = 0;

    match fork() {
        Ok(ForkResult::Parent { child, .. }) => {
            println!(
                "Continuing execution in parent process, new child has pid: {}",
                child
            );

            // parent goes down this path (original process)
            let mut w_status: i32;
            // on success wait returns the PID of the terminated child
            // on error, -1 is returned
            // int wc = wait(NULL);
            let wc: i32 = nix::sys::wait::waitpid(&mut w_status);
            println!("hello, I am parent of {} (wc:{}, child terminated normally:{}, child terminated by a signal:{}) (pid:{})", child, child, nix::sys::wait::WaitStatus::Exited(w_status), nix::sys::wait::WaitStatus::Signaled(w_status), std::process::id());
        }
        Ok(ForkResult::Child) => {
            println!("I'm a new child process");
            sleep(1);
        }
        Err(_) => panic!("Fork failed"),
    }
}
