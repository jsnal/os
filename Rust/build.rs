use std::{env, process::Command};

fn main() {
    println!("cargo::rustc-env=TARGET={}", env::var("TARGET").unwrap());
    println!("cargo::rustc-check-cfg=cfg(dtb)");

    let out_dir = env::var("OUT_DIR").unwrap();
    let arch_str = env::var("CARGO_CFG_TARGET_ARCH").unwrap();

    match &*arch_str {
        "x86" => {
            println!("cargo::rerun-if-changed=src/boot.asm");

            let status = Command::new("nasm")
                .arg("-f")
                .arg("elf32")
                .arg("-o")
                .arg(format!("{}/boot.o", out_dir))
                .arg("src/boot.asm")
                .status()
                .expect("failed to run nasm");
            if !status.success() {
                panic!("nasm failed with exit status {}", status);
            }
        }
        _ => {}
    }
}
