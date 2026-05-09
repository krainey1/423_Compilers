// EXPECT: semantic error — if condition is not Boolean
fun main() {
    if (42) {
        println("bad")
    }
}
