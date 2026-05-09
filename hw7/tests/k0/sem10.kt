// sem10.kt: elvis operator with nullable string
fun main() {
    var s: String? = null
    val result = s ?: "fallback"
    println(result)
}
