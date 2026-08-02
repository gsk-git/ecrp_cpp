# Testing Guide for ESRO

This guide explains how to write, run, and understand the sample tests in this project using **Catch2**.

---

## 📋 Quick Start

### 1. **Enable Testing in CMake**

Before building, you need to enable tests:

```powershell
# Using CMakePresets or Visual Studio
# Select the x64-Debug or x64-Release configuration

# From command line:
cmake --preset x64-Debug -DBUILD_TESTS=ON
cmake --build --preset x64-Debug
```

Or directly in Visual Studio:
- Right-click the project → Properties
- Set `BUILD_TESTS=ON`
- Rebuild

### 2. **Run the Tests**

After building with `BUILD_TESTS=ON`, you'll have an executable: `ESRO_tests` (or similar)

```powershell
# Navigate to build directory
cd out/build/x64-Debug

# Run all tests
./bin/ESRO_tests.exe

# Run tests matching a specific tag
./bin/ESRO_tests.exe "[math]"
./bin/ESRO_tests.exe "[player]"

# Run with verbose output
./bin/ESRO_tests.exe -v
```

### 3. **Using Visual Studio Test Explorer**

If using Visual Studio's built-in Test Explorer:
- View → Test Explorer
- Should auto-discover tests
- Right-click and select "Run Selected Tests"

---

## 🧪 Test Structure Explained

### **TEST_CASE Macro**
```cpp
TEST_CASE("Test name", "[tag1][tag2]")
{
	// Test code here
}
```
- **Test name**: Human-readable description
- **Tags**: Optional categories for organizing/filtering tests (use `[player]`, `[math]`, etc.)

### **CHECK vs REQUIRE**
```cpp
CHECK(condition);      // Continues even if fails
REQUIRE(condition);    // Stops test if fails
```

---

## 📚 Sample Tests Breakdown

### **1. Basic Arithmetic Tests**
```cpp
TEST_CASE("Basic arithmetic operations", "[math]")
{
	CHECK(5 + 3 == 8);
	CHECK(5 - 3 == 2);
}
```
**When to use**: Testing simple functions, math operations
**Run it**: `./ESRO_tests "[math]"`

---

### **2. Vector/Container Tests**
```cpp
TEST_CASE("Vector operations", "[vector]")
{
	std::vector<int> vec;

	SECTION("Empty vector")
	{
		CHECK(vec.empty());
	}

	SECTION("After adding elements")
	{
		vec.push_back(1);
		CHECK(vec.size() == 1);
	}
}
```
**When to use**: Testing collections, data structures
**Why SECTION?**: Tests multiple scenarios for same object

---

### **3. Exception Tests**
```cpp
TEST_CASE("Exception handling", "[exceptions]")
{
	CHECK_THROWS_AS(divide(10, 0), std::invalid_argument);
}
```
**When to use**: Verifying error conditions
**Other options**:
- `CHECK_THROWS` - any exception
- `CHECK_NOTHROW` - no exception thrown

---

### **4. Player Class Tests**
```cpp
TEST_CASE("Player class", "[player]")
{
	Player player("Hero", 100, 0);

	SECTION("Take damage")
	{
		player.take_damage(30);
		CHECK(player.health == 70);
	}
}
```
**When to use**: Testing your game classes
**How to extend**: Add sections for different behaviors

---

### **5. Parametrized Tests (Run Multiple Times)**
```cpp
TEST_CASE("Parametrized calculations", "[parametrized]")
{
	int x = GENERATE(1, 2, 3, 4, 5);
	CHECK(x > 0);
	// This runs 5 times with x = 1, 2, 3, 4, 5
}
```

**Table-driven variant:**
```cpp
auto [input, expected] = GENERATE(
	std::make_tuple(2, 4),
	std::make_tuple(3, 9),
	std::make_tuple(5, 25)
);
```
**When to use**: Same test logic with multiple inputs

---

## 🎯 Writing Your Own Tests

### **Template for New Tests**

```cpp
TEST_CASE("What you're testing", "[category]")
{
	// 1. Setup
	MyClass obj("initial", 42);

	// 2. Action
	obj.doSomething();

	// 3. Assert
	CHECK(obj.getValue() == 42);
}
```

### **Example: Testing Chunk Manager**

```cpp
TEST_CASE("ChunkManager", "[world]")
{
	ChunkManager cm;

	SECTION("Generate chunk")
	{
		cm.generateChunk(0, 0, 12345);
		CHECK(cm.chunkExists(0, 0));
	}

	SECTION("Unload chunk")
	{
		cm.generateChunk(0, 0, 12345);
		cm.unloadChunk(0, 0);
		CHECK_FALSE(cm.chunkExists(0, 0));
	}
}
```

### **Example: Testing PlayerManager**

```cpp
TEST_CASE("PlayerManager", "[player]")
{
	PlayerManager pm;

	SECTION("Add player")
	{
		pm.addPlayer("Alice");
		CHECK(pm.getPlayerCount() == 1);
	}

	SECTION("Remove player")
	{
		pm.addPlayer("Bob");
		pm.removePlayer("Bob");
		CHECK(pm.getPlayerCount() == 0);
	}
}
```

---

## 🔧 Useful Catch2 Assertions

| Macro | Purpose |
|-------|---------|
| `CHECK(x)` | Assert x is true (non-fatal) |
| `REQUIRE(x)` | Assert x is true (fatal if fails) |
| `CHECK_FALSE(x)` | Assert x is false |
| `CHECK_THROWS_AS(expr, exception)` | Expect exception |
| `CHECK_NOTHROW(expr)` | Expect no exception |
| `CHECK_EQUALS(a, b)` | Exact comparison |
| `CHECK(x == Approx(y))` | Float comparison |

---

## 📊 Running Tests from Command Line

```powershell
# Run all tests
./ESRO_tests.exe

# Run specific test by name
./ESRO_tests.exe "Basic arithmetic"

# Run by tag
./ESRO_tests.exe "[player]"
./ESRO_tests.exe "[math] or [physics]"

# Verbose output
./ESRO_tests.exe -v

# Show available tests without running
./ESRO_tests.exe --list-tests

# XML output (for CI/CD)
./ESRO_tests.exe -o results.xml
```

---

## 🚀 Best Practices

✅ **Do:**
- One assertion concept per test
- Use descriptive test names
- Test edge cases (0, negative, max values)
- Use SECTION for related test scenarios
- Keep tests independent (no dependencies between tests)

❌ **Don't:**
- Mix multiple unrelated assertions
- Use vague test names like "test1"
- Test implementation details (test behavior instead)
- Make tests depend on execution order

---

## 🐛 Common Issues

**Issue**: Tests don't compile
- Solution: Make sure `BUILD_TESTS=ON` in CMake

**Issue**: Can't find Catch2
- Solution: CMake will auto-fetch it on first build

**Issue**: Tests pass locally but fail in CI
- Reason: Platform differences, floating point precision
- Solution: Use `Approx()` for floats, handle platform differences

---

## 📝 CMakeLists.txt Configuration

Your project is already configured to:
1. Fetch Catch2 from GitHub automatically
2. Look for tests in `tests/tests.cpp`
3. Create `ESRO_tests` executable
4. Register with CTest (run via `ctest`)

To run via CTest:
```powershell
cd out/build/x64-Debug
ctest -V
```

---

## 🎓 Next Steps

1. Add tests for your **PlayerManager** class
2. Add tests for your **ChunkManager** class
3. Add tests for **WorldManager** 
4. Test edge cases (empty worlds, extreme coordinates)
5. Set up CI/CD to run tests automatically

---

**Happy Testing! 🎉**
