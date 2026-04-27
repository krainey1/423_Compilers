// EXPECT: semantic error — duplicate function name
fun foo() { println("first") }
fun foo() { println("second") }
fun main() { foo() }
