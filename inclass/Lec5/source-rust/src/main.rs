use std::io;

use nix::unistd::*;

fn main() {
    println!("hello world (pid:{})\n", std::process::id());
    let shared_int = 0;

    if let Some(ForkResult::Child) = fork() {
        println!("hello");
    }
}
