// EXPECT: semantic error — reassigning typed val
fun main() {
    val name: String = "alice"
    name = "bob"
}
