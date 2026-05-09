// EXPECT: semantic error — too many arguments
fun greet(name: String) {
    println(name)
}
fun main() {
    greet("alice", "extra")
}
