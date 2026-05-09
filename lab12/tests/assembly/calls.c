///to cover OP_PARM, OP_CALL, OP_RETURN

int add(int a, int b) {
        return a + b;
}

void test() {
        int r = add(3,4);
}