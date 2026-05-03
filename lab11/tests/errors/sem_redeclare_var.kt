// EXPECT: semantic error — redeclaring same name in same scope
fun main() {
    var x: Int = 1
    var x: Int = 2
}
