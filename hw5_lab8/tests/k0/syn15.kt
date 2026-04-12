// syn15.kt: function with array parameter
fun sum(arr: Array<Int>): Int {
    var total = 0
    for (i in 0..2) {
        total += arr[i]
    }
    return total
}
fun main() {
    val arr = arrayOf(1, 2, 3)
    val s = sum(arr)
}
