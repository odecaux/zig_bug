const std = @import("std");
const Builder = std.build.Builder;

pub fn build(b: *Builder) void {
    const target = b.standardTargetOptions(.{});
    const mode = b.standardReleaseOptions();

    const exe = b.addExecutable("dsp_bench", null);
    exe.setTarget(target);
    exe.setBuildMode(mode);
    if(mode == .Debug)
    {
        exe.defineCMacro("DEBUG", "");
    }
    else 
    {
        exe.defineCMacro("RELEASE", "");
    }
    exe.addCSourceFiles(&.{ 
        "main.cpp"}, &.{
        "-std=c++20", "-gcodeview", "-fno-exceptions", "-ffast-math",  "-fno-rtti"
    });
    exe.linkLibC();
   
     exe.linkSystemLibrary("User32");
    exe.linkSystemLibrary("Ole32");
    exe.linkSystemLibrary("opengl32");
    exe.linkSystemLibrary("Gdi32");
    exe.linkSystemLibrary("Comdlg32");
    exe.want_lto = false;

    exe.defineCMacro("STRSAFE_NO_DEPRECATE", "");
    exe.defineCMacro("_CRT_SECURE_NO_WARNINGS", "");
    exe.install();
}
