/* Simulates a physical half adder. Uses and AND gate and an XOR gate in order
 * to represent this operation. Call execute() to get the output of the adder.
 *
 * Author: Jacob Scott
 */
public class Sim2_HalfAdder {

	/* 
	 * Runs the half adder on the inputs
	 * 
	 * input: a, b
	 * 
	 * output: sum, carry
	 */
	public void execute() {
		
		xor.a.set(a.get());
		xor.b.set(b.get());
		
		and.a.set(a.get());
		and.b.set(b.get());
		
		and.execute();
		xor.execute();
		
		sum.set(xor.out.get());
		carry.set(and.out.get());
	}
	
	// inputs
	public RussWire a,b;
	// outputs
	public RussWire sum, carry;
	
	// internal components
	public AND and;
	public Sim2_XOR xor;
	
	/* 
	 * Constructor for the half adder
	 */
	public Sim2_HalfAdder() {
		a   = new RussWire();
		b   = new RussWire();

		sum = new RussWire();
		carry = new RussWire();

		and = new AND();
		xor = new Sim2_XOR();
	}
}
