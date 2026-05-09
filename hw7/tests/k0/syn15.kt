// syn15.kt: function with typed parameters and return
fun sum(a: Int, b: Int, c: Int): Int {
    var total = 0
    total += a
    total += b
    total += c
    return total
}
fun main() {
    val s = sum(1, 2, 3)
    println(s)
}
