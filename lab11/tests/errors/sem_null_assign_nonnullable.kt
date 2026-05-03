// EXPECT: semantic error — assigning null to non-nullable after declaration
fun main() {
    var s: String = "hello"
    s = null
}
