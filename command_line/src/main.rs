use std::env;
use std::process::exit;

mod commands;
use crate::commands::put::put_handler;
use crate::commands::get::get_handler;


//TODO Set the password. Delete all files on SD if you are resetting it
fn set_handler() {println!("Running set handler")}

//TODO Show all files on the SD card
fn list_handler() {}

fn main() {
    println!("Hello, world!");

// Use this to see what port it is on somehow
//    let x = available_ports().unwrap();
//
//    for i in x {
//        println!("{}", i.port_name);
//        println!("{:?}", i.port_type);
//    }

    let args: Vec<String> = env::args().collect();
    //TODO replace unwrap to state there needs to be input if it errors
    let command = args.get(1).unwrap().to_uppercase();


    if command == String::from("SET") {
        set_handler();
    }
    else if command == String::from("PUT") {
        put_handler(args.get(2).unwrap().parse().unwrap());
    }
    else if command == String::from("GET") {
        get_handler();
    }
    else if command == String::from("LIST") {
        list_handler();
    }
    else {
        println!("ERROR: Unrecognized command {}", command);
        exit(0);
    }
}

//TODO Once we have get functions working, these tests will put & get then compare the two
//For right now, one must check the files manually on the SD card
//cargo test -- --test-threads=1
#[cfg(test)]
mod tests {
    use crate::put_handler;

    #[test]
    fn put_single_line_file() {
        assert!(put_handler(String::from("single_line.txt")), true);

    }
    #[test]
    fn put_multi_line_file() {
        assert!(put_handler(String::from("multi_line.txt")), true);
    }
}
