// EXPECT: semantic error — while condition is not Boolean
fun main() {
    while ("keep going") {
        println("loop")
    }
}
