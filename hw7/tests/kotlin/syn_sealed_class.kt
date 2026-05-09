// Kotlin feature: sealed class (not in k0)
sealed class Shape
class Circle(val radius: Double) : Shape()
class Rect(val w: Double, val h: Double) : Shape()
fun main() { }
