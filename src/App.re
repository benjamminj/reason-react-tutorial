type repository = {
  name: string,
  description: string,
  href: string,
};

type state = {
  input: string,
  loading: bool,
};

type action =
  | UpdateInput(string)
  | Search;

let component = ReasonReact.reducerComponent("App");

module Api = {
  open Json.Decode;

  let decodeResults = field("items", array(json => {
    name: field("name", string, json),
    description: field("description", string, json),
    href: field("html_url", string, json),
  }));
  
  let getResults = query => 
    Js.Promise.(
      Fetch.fetch("https://api.github.com/search/repositories?q=" ++ query)
      |> then_(Fetch.Response.json)
      |> then_(json => {
        Js.log(json);
        resolve();
      })
    );
    
};

let make = _children => {
  ...component,
  initialState: () => {input: "", loading: false},
  reducer: (action, state) =>
    switch (action) {
    | UpdateInput(input) => ReasonReact.Update({...state, input})
    | Search =>
      ReasonReact.UpdateWithSideEffects(
        {...state, loading: true},
        (
          self => {
            let value = self.state.input;
            let _promise = Api.getResults(value);
            ();
          }
        ),
      )
    },
  render: self =>
    <div>
      <form
        onSubmit={
          ev => {
            ReactEvent.Form.preventDefault(ev);
            self.send(Search);
          }
        }>
        <label htmlFor="search"> {ReasonReact.string("Search")} </label>
        <input
          id="search"
          name="search"
          value={self.state.input}
          onChange={
            ev => {
              let value = ReactEvent.Form.target(ev)##value;
              self.send(UpdateInput(value));
            }
          }
        />
        <button type_="submit">
          {ReasonReact.string("Submit Search")}
        </button>
      </form>
      <div>
        {
          self.state.loading ?
            ReasonReact.string("Loading...") : ReasonReact.null
        }
      </div>
    </div>,
};
