# UTG (Ultrasonic Thickness Gauge) Testing Documentation

This document describes the comprehensive testing suite for the UTG implementation in QGroundControl.

## Test Structure Overview

The UTG testing suite consists of three main test categories:

1. **Unit Tests** - Test individual components in isolation
2. **Integration Tests** - Test component interactions and workflows
3. **Mock Device Tests** - Test with simulated UTG hardware

## Test Files

### Unit Tests

#### `UTGSettingsTest` (`src/Settings/UTGSettingsTest.h/cc`)
Tests the UTGSettings class functionality:

- **Default Values**: Verifies all 23 settings have correct default values
- **Settings Persistence**: Tests that settings are saved and loaded correctly
- **Serial Port Settings**: Tests baud rate, data bits, stop bits, parity, flow control
- **Measurement Settings**: Tests units, modes, velocity, gain, threshold, etc.
- **Calibration Settings**: Tests calibration modes, material types, reference values
- **Alert Settings**: Tests threshold alerts and logging configuration
- **Validation**: Tests input validation and error handling
- **Reset to Defaults**: Tests settings reset functionality
- **Signal Emission**: Tests that appropriate signals are emitted
- **Boundary Values**: Tests minimum and maximum value handling
- **Material Presets**: Tests material type selection

#### `UTGManagerTest` (`src/Vehicle/UTGManagerTest.h/cc`)
Tests the UTGManager class functionality:

- **Initialization**: Tests proper initialization and initial state
- **Connection Management**: Tests connect/disconnect lifecycle
- **Command Building**: Tests protocol command construction
- **Response Parsing**: Tests protocol response handling
- **Measurement Commands**: Tests single and continuous measurements
- **Calibration Commands**: Tests zero, velocity, and material calibration
- **Parameter Commands**: Tests gain, velocity, threshold, etc. setting
- **Error Handling**: Tests error detection and recovery
- **Status Management**: Tests status transitions and reporting
- **Signal Emission**: Tests that appropriate signals are emitted
- **Settings Integration**: Tests response to settings changes
- **Protocol Validation**: Tests communication protocol compliance
- **Communication Timeout**: Tests timeout handling and retry logic
- **Serial Port Configuration**: Tests serial port setup

### Integration Tests

#### `UTGIntegrationTest` (`src/Vehicle/UTGIntegrationTest.h/cc`)
Tests the complete UTG system integration:

- **Full Workflow**: Tests complete settings-to-measurement workflow
- **Settings-Manager Integration**: Tests settings changes affecting UTGManager
- **Measurement Workflow**: Tests end-to-end measurement process
- **Calibration Workflow**: Tests complete calibration procedures
- **Error Recovery**: Tests system recovery from various error conditions
- **Continuous Measurement**: Tests continuous measurement mode
- **Parameter Synchronization**: Tests settings sync with device
- **Connection Lifecycle**: Tests complete connection/disconnection cycle
- **Material Presets**: Tests material type changes and velocity updates
- **Alert System**: Tests threshold alerts and notifications

### Mock Device

#### `MockUTGDevice` (`src/qgcunittest/MockUTGDevice.h/cc`)
Simulates a real UTG device for testing:

- **Command Processing**: Simulates device command handling
- **Response Generation**: Generates realistic device responses
- **Error Simulation**: Can simulate various error conditions
- **State Management**: Maintains device state (idle, measuring, calibrating)
- **Configuration Tracking**: Tracks device parameter changes
- **Measurement Simulation**: Generates realistic thickness measurements
- **Protocol Compliance**: Implements the UTG communication protocol
- **Timing Simulation**: Simulates realistic response delays

## Test Coverage

### Settings Testing
```
✅ Default value verification (23 parameters)
✅ Value persistence across sessions
✅ Input validation and range checking
✅ Signal emission on changes
✅ Reset to defaults functionality
✅ Material preset handling
✅ Serial port configuration
✅ Measurement parameter validation
✅ Calibration setting validation
✅ Alert threshold validation
```

### Manager Testing
```
✅ Initialization and cleanup
✅ Connection state management
✅ Command queue handling
✅ Protocol message building
✅ Response parsing and validation
✅ Error detection and reporting
✅ Status transitions
✅ Measurement processing
✅ Calibration procedures
✅ Parameter synchronization
```

### Integration Testing
```
✅ End-to-end workflows
✅ Settings-to-device communication
✅ Real-time measurement handling
✅ Calibration procedures
✅ Error recovery scenarios
✅ Continuous measurement mode
✅ Material preset changes
✅ Alert system functionality
✅ Connection lifecycle management
✅ Multi-component interaction
```

## Running Tests

### Build Tests
```bash
# Build QGroundControl with tests enabled
mkdir build && cd build
cmake .. -DQGC_BUILD_TESTING=ON
make -j4
```

### Run All UTG Tests
```bash
# Run all UTG-related tests
./qgroundcontrol --unittest UTGSettingsTest
./qgroundcontrol --unittest UTGManagerTest
./qgroundcontrol --unittest UTGIntegrationTest
```

### Run Individual Test Methods
```bash
# Run specific test methods
./qgroundcontrol --unittest UTGSettingsTest::_testDefaultValues
./qgroundcontrol --unittest UTGManagerTest::_testConnectionManagement
./qgroundcontrol --unittest UTGIntegrationTest::_testFullWorkflow
```

### Run Tests with Verbose Output
```bash
# Enable detailed test output
./qgroundcontrol --unittest UTGSettingsTest -v2
```

## Test Data and Scenarios

### Test Scenarios Covered

#### Normal Operation
- Device connection and initialization
- Single thickness measurements
- Continuous measurement mode
- Parameter adjustments
- Material type changes
- Calibration procedures

#### Error Conditions
- Connection failures
- Communication timeouts
- Invalid responses
- Device errors
- Serial port issues
- Protocol violations

#### Edge Cases
- Boundary value testing
- Rapid parameter changes
- Connection interruptions
- Invalid input handling
- Resource cleanup

#### Performance Testing
- Response time measurement
- Memory usage validation
- Signal emission timing
- Command queue efficiency

## Mock Device Capabilities

The MockUTGDevice provides comprehensive simulation:

### Realistic Behavior
- Proper protocol implementation
- Configurable response delays
- State-dependent responses
- Error condition simulation
- Measurement variation simulation

### Test Control
- Command tracking and verification
- Response injection
- Error rate configuration
- State manipulation
- Performance monitoring

### Protocol Compliance
- Correct message formatting
- Checksum validation
- Command acknowledgment
- Error response generation
- Timing simulation

## Continuous Integration

### Automated Testing
The tests are designed to run in CI/CD environments:

- No external hardware dependencies
- Deterministic test results
- Comprehensive error reporting
- Performance benchmarking
- Code coverage analysis

### Test Reports
Tests generate detailed reports including:

- Pass/fail status for each test
- Performance metrics
- Error condition coverage
- Protocol compliance verification
- Memory usage analysis

## Test Maintenance

### Adding New Tests
When adding new UTG functionality:

1. Add unit tests for new components
2. Update integration tests for new workflows
3. Extend mock device for new commands
4. Update test documentation
5. Verify CI/CD integration

### Test Data Management
- Use realistic test values
- Cover boundary conditions
- Test error scenarios
- Validate performance requirements
- Maintain test data consistency

## Debugging Tests

### Common Issues
- Mock device not responding: Check connection simulation
- Test timeouts: Verify signal/slot connections
- Protocol errors: Validate message formatting
- State inconsistencies: Check initialization order

### Debug Tools
- QTest framework debugging
- Signal spy monitoring
- Mock device logging
- Protocol message inspection
- Performance profiling

## Performance Benchmarks

### Expected Performance
- Settings load time: < 100ms
- Connection establishment: < 2s
- Single measurement: < 500ms
- Parameter change: < 200ms
- Calibration procedure: < 5s

### Memory Usage
- UTGSettings: < 1KB
- UTGManager: < 10KB
- Mock device: < 5KB
- Total test overhead: < 50KB

This comprehensive testing suite ensures the UTG implementation is robust, reliable, and ready for production use.
