function test(n) {
    console.log("============ TEST #" + n + " ============");
    sys.exec_file("..\\test\\test_" + n + ".js");
}

test(8);
test(9);
return;