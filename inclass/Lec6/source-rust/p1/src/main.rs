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

            println!(
                "hello, I am parent of {} (pid:{}) and the shared data is {}",
                child,
                std::process::id(),
                shared_int
            );
            shared_int -= 1;
            println!(
                "hello, I am parent of {} (pid:{}) and the modified shared data is {}",
                child,
                std::process::id(),
                shared_int
            );
        }
        Ok(ForkResult::Child) => {
            println!("I'm a new child process");

            println!(
                "hello, I am child (pid:{}) and the shared data is {}",
                std::process::id(),
                shared_int
            );
            shared_int += 1;
            println!(
                "hello, I am child (pid:{}) and the modified shared data is {}",
                std::process::id(),
                shared_int
            );
        }
        Err(_) => panic!("Fork failed"),
    }
}
