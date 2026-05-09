// Kotlin feature: extension function (not in k0)
fun String.shout(): String = this.uppercase() + "!"
fun main() {
    println("hello".shout())
}
