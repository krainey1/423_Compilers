// Kotlin feature: companion object
class MyClass {
    companion object {
        fun create() = MyClass()
    }
}
fun main() {
    val obj = MyClass.create()
}
