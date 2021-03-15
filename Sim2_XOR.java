/* Simulates a physical XOR gate. Uses two AND gates and an OR gate 
 * as well as a not to represent the XOR operation. Call execute() 
 * to get the output of the gate.
 *
 * Author: Jacob Scott
 */
public class Sim2_XOR {

	/* 
	 * Runs the XOR gate on the inputs
	 * 
	 * input: a, b
	 * 
	 * output: out
	 */
	public void execute() {
		
		andOne.a.set(a.get());
		andOne.b.set(b.get());
		
		or.a.set(a.get());
		or.b.set(b.get());
		
		andOne.execute();
		or.execute();
		
		not.in.set(andOne.out.get());
		
		not.execute();
		
		andTwo.a.set(or.out.get());
		andTwo.b.set(not.out.get());
		
		andTwo.execute();
		
		out.set(andTwo.out.get());
	}
	
	// inputs
	public RussWire a,b;
	// outputs
	public RussWire out;
	
	// internal components
	public AND andOne;
	public AND andTwo;
	public NOT not;
	public OR or;
	
	/* 
	 * Constructor for the XOR gate
	 */
	public Sim2_XOR() {
		a   = new RussWire();
		b   = new RussWire();

		out = new RussWire();

		andOne = new AND();
		andTwo = new AND();
		not = new NOT();
		or = new OR();
	}
}
