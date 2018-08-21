import React, { Component } from 'react';
import logo from './logo.svg';
import './App.css';

class App extends Component {
  constructor(props) {
    super(props);
    this.state = {
      msg: "",
      data: "",
      method: "GET"
    }
    this.handleOnChange = (e) => {
      this.setState({
        msg: e.target.value
      })
    }
    this.methodChange = (e) => {
      this.setState({
        method: e.target.value
      })
    }
    this.send = () => {
      if (["GET", "HEAD"].indexOf(this.state.method.toUpperCase())==-1) {
        fetch('/req-1', {
          method: this.state.method,
          body: this.state.msg
        })
        .then( raw => raw.text())
        .then( result => {
          this.setState({
            data: result
          })
        })  
      }
      else {
        fetch('/req-2')
        .then( raw => raw.text())
        .then( result => {
          this.setState({
            data: result
          })
        })
      }
    }

  }
  render() {
    return (
      <div className="App">
        <header className="App-header">
          <h1 className="App-title">Demo mini project</h1>
        </header>
        <div className="demo-input-form">
          <input placeholder="Leave a message" className="demo-input" type="text" value={this.state.msg} onChange={this.handleOnChange}/>
          <input placehoder="Type a HTTP method" className="demo-send-btn" type="text" value={this.state.method} onChange={this.methodChange}/>
          <button className="demo-send-btn" onClick={this.send}>send</button>
        </div>
        <div className="demo-output-form">
          <textarea className="demo-output" value={this.state.data}/>
        </div>
      </div>
    );
  }
}

export default App;
