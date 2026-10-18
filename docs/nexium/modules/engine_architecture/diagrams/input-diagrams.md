# Input diagram sources

The eleven images/input-*.svg files are editable vector sources used directly
by Antora. They have no external diagram renderer or raster-image dependency.
Each SVG includes a title/description for accessibility and keeps its text as text.

| Asset | Contract illustrated |
| --- | --- |
| input-pipeline.svg | Collection, physical state, routing, controller-local runtime, consumers |
| input-binding-data.svg | Configuration copies and independent action runtimes |
| input-action-timeline.svg | Press/held/release rules and same-frame taps |
| input-context-stack.svg | Overlay/global/base order and channel-specific blocking |
| input-cancellation.svg | Silent cancellation, held-button quarantine, fresh acceptance |
| input-pointer-capture.svg | Accepted mouse gesture ownership and interruption |
| input-controller-hierarchy.svg | Domain inheritance and separate InputController inheritance |
| input-controller-ownership.svg | Strong child ownership, weak possession, non-owning registration |
| input-frame-sequence.svg | Runtime poll/policy/process/tick/UI order |
| input-serialization-flow.svg | Saved configuration versus runtime relationships |
| input-recording-capture.svg | Recording guard and pending-release fence |

Hollow triangles mean inheritance and point to the base. Ordinary arrows mean
flow or relationships as labeled; dashed arrows in the ownership diagram are weak
or non-owning references. Blue marks input/configuration, green routing/active
state, purple domain/editor behavior, red blocking/cancellation, and gray physical
or inactive state.

Update diagrams alongside source contracts and narrative pages. The context stack
is illustrative rather than a fixed editor preset; the actual policy is documented
with a live include from EditorIntegration. Each timing row represents a separate
map, because binding resolution chooses one winner per component/event.

Render source SVGs when checking layout. Never use generated site HTML as the
source for diagram facts or edit it by hand.
