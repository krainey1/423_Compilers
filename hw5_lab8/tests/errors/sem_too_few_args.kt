// EXPECT: semantic error — too few arguments
fun add(a: Int, b: Int): Int {
    return a + b
}
fun main() {
    val r = add(1)
}
